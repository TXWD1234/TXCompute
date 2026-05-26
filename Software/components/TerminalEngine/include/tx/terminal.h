// Copyright (c) 2026 TXCompute. Licensed under the MIT License.

#pragma once
#include "tx/math.h"
#include "tx/data.h"
#include "tx/esp.h"
#include <algorithm>


/**
 * The terminal engine is the very end of the TXESP infrastructure, combining
 * keyboard input and screen / display output
 */



namespace tx::terminal {

/**
 * Architecture Design Pattern:
 * Logics are separated into different classes, each are friend with whoever
 * going to use it. It will contain a pointer to the object class that it
 * belongs to, as it's parent, and modify data there.
 * 
 * The terminal engine is designed under the KISS rule:
 * Keep it Simple, Stupid
 */



template <class T>
struct Buffer {
	Buffer(tx::u32 size)
	    : buf(size, static_cast<T*>(heap_caps_malloc(
	                    size, MALLOC_CAP_8BIT))) {}
	~Buffer() {
		heap_caps_free(buf.data());
	}

	tx::StaticGrowArr<T> buf;
};




/**
 * @note
 * this class will allocate memory:
 * - string buffer - size of provided capacity
 * - metadata buffer - size of provided max string count
 */
class StringPool {
public:
	StringPool(u32 capacity, u32 maxStringCount)
	    : m_data(capacity) {}

private:
	// basically std::string_view
	struct Range_impl {
		u32 offset, size;
	};

private:
	Buffer<char> m_data;
};

class TextBuffer {
};

/**
 * Link StringPool and TextBuffer together, for the overflow of StringPool
 * can take affect at TextBuffer
 */
class TextBufferManager {
};



/**
 * @note
 * this class will allocate memory:
 * - double text buffer (left and right buffer) of the max buffer size provided
 * 
 */
class InputLine {
public:
	InputLine(tx::u32 maxBufferSize)
	    : m_data(maxBufferSize) {}

	// user operations

	void input(char val) {
		left_impl().push_back(val);
	}
	// backspace
	void deleteFront() {
		left_impl().pop_back();
	}
	// delete
	void deleteBack() {
		right_impl().pop_back();
	}
	void cursorMoveLeft(tx::u32 distance = 1) {
		for (tx::u32 i = 0; i < distance && left_impl().size(); i++) {
			right_impl().push_back(left_impl().back());
			left_impl().pop_back();
		}
	}
	void cursorMoveRight(tx::u32 distance = 1) {
		for (tx::u32 i = 0; i < distance && right_impl().size(); i++) {
			left_impl().push_back(right_impl().back());
			right_impl().pop_back();
		}
	}

	// program operations

	// @return write successful
	bool output(std::span<char> buffer) {
		// size check
		if (buffer.size() < left_impl().size() + right_impl().size()) return false;
		std::reverse_copy(right_impl().begin(), right_impl().end(),
		                  std::copy(left_impl().begin(), left_impl().end(), buffer.begin()));
		return true;
	}

	void clear() {
		left_impl().clear();
		right_impl().clear();
	}


private:
	struct Data_impl {
		Data_impl(tx::u32 size)
		    : left(size),
		      right(size) {}

		Buffer<char> left;
		Buffer<char> right;
	} m_data;

	tx::StaticGrowArr<char>& left_impl() {
		return m_data.left.buf;
	}
	tx::StaticGrowArr<char>& right_impl() {
		return m_data.right.buf;
	}
};



class InputHandler {
public:
private:
private:
};










struct Font {
	tx::u32 width;
	tx::u32 height;
	std::span<tx::u8> bitmapData;
};
inline TextRenderer makeTextRenderer(Font font) {
	return TextRenderer{
		font.width,
		font.height,
		font.bitmapData
	};
}



/**
 * @note
 * this class will allocate memory:
 * - TextRenderer: double render buffer, each `width * height * 2` (u16)
 * - String buffer: 
 */
class TerminalEngine {
	/**
	 * This Terminal Engine is highly optimized for memory usage, but not performance.
	 * Both the update and render logic is designed with tight memory constraint in mind
	 */
public:
	// type def

	using InputCallback_t = std::function<void(std::string_view)>;

public:
	TerminalEngine(Font font) : m_rr(makeTextRenderer(font)) {}

	// init data setter

	void setInputCallback(InputCallback_t cb) {
		m_inputCb = cb;
		m_inputCbInited = true;
	}
	template <std::invocable<std::string_view> Func>
	void setInputCallback(Func&& f) {
		m_inputCb = std::forward<Func>(f);
		m_inputCbInited = true;
	}

private:
	// call back and source data (init data)

	InputCallback_t m_inputCb;
	bool m_inputCbInited = false;

	std::span<std::string_view> m_stringPresets;

private:
	// runtime data

	TextRenderer m_rr;
};
} // namespace tx::terminal