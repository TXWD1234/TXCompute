// Copyright (c) 2026 TXCompute. Licensed under the MIT License.

#pragma once
#include "esp_heap_caps.h"
#include "impl/grow_array.hpp"
#include <span>

namespace tx::esp {

template <class T>
inline T* allocate(u32 size) {
	if (size == 0) return nullptr;
	return static_cast<T*>(heap_caps_malloc(
	    size * sizeof(T), MALLOC_CAP_8BIT));
}
template <class T>
inline void free(T* ptr) {
	if (!ptr) return;
	heap_caps_free(ptr);
}


template <class T>
struct Buffer {
public:
	Buffer(u32 capacity)
	    : m_data(allocate<T>(capacity)),
	      m_size(capacity) {}
	~Buffer() {
		free(m_data);
	}

	T* data() { return m_data; }
	const T* data() const { return m_data; }

	u32 size() const { return m_size; }

	std::span<T> span() { return std::span<T>(m_data, m_size); }
	std::span<const T> span() const { return std::span<const T>(m_data, m_size); }

private:
	T* m_data;
	u32 m_size;
};

template <class T>
class Buffer_GrowArray : public GrowArrayOverlay<T> {
public:
	Buffer_GrowArray(u32 capacity)
	    : GrowArrayOverlay<T>(
	          allocate<T>(capacity), capacity) {}
	~Buffer_GrowArray() {
		if (!this->isNull_impl()) {
			this->destruct_impl(); // destroy live elements before freeing
			free(this->data());
		}
	}

	Buffer_GrowArray(const Buffer_GrowArray<T>& other)
	    : GrowArrayOverlay<T>(
	          allocate<T>(other.m_capacity), other.m_capacity) {
		copy_impl(other);
	}
	Buffer_GrowArray(Buffer_GrowArray<T>&& other) : GrowArrayOverlay<T>(other) {
		// just use the copy constructor of GrowArrayOverlay - shallow copy
		other.null_impl();
	}
	Buffer_GrowArray& operator=(Buffer_GrowArray<T> other) {
		this->swap_impl(other);
		return *this;
	}

private:
	// cannot be swap_impl because ambiguity with base's swap_impl
	// this function exists for potential future expansion
	void swap(Buffer_GrowArray<T>& other) {
		swap_impl(other);
	}
};

template <class T>
class Buffer_CircularQueue : public CircularQueueOverlay<T> {
public:
	Buffer_CircularQueue(u32 capacity)
	    : CircularQueueOverlay<T>(
	          allocate<T>(capacity), capacity) {}
	~Buffer_CircularQueue() {
		if (!this->isNull_impl()) {
			this->destruct_impl(); // destroy live elements before freeing
			free(this->data());
		}
	}

	Buffer_CircularQueue(const Buffer_CircularQueue<T>& other)
	    : CircularQueueOverlay<T>(
	          allocate<T>(other.capacity()), other.capacity()) {
		copy_impl(other);
	}
	Buffer_CircularQueue(Buffer_CircularQueue<T>&& other) : CircularQueueOverlay<T>(other) {
		// just use the copy constructor of CircularQueueOverlay - shallow copy
		other.null_impl();
	}
	Buffer_CircularQueue& operator=(Buffer_CircularQueue<T> other) {
		this->swap_impl(other);
		return *this;
	}

private:
	// cannot be swap_impl because ambiguity with base's swap_impl
	// this function exists for potential future expansion
	void swap(Buffer_CircularQueue<T>& other) {
		swap_impl(other);
	}
};
} // namespace tx::esp