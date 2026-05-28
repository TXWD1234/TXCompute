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

	tx::StaticGrowArr<T>* operator->() { return &buf; }
	const tx::StaticGrowArr<T>* operator->() const { return &buf; }

	tx::StaticGrowArr<T> buf;
};



/**
 * @note
 * this class will allocate memory:
 * - string buffer - size of provided capacity
 * - metadata buffer - size of provided max string count
 * 
 * manages the shrink logic when buffer is full (eviction)
 */
class StringPool {
	/**
	 * Terminology:
	 * - Evict:
	 *   The operation of deleting old data to make space for incoming new data,
	 *   when buffer is full
	 * - Object / Meta / Range:
	 *   These terms refers to an entry in m_meta, which is corresponding to a
	 *   range of data in m_data. Conceptually simillar to a partition
	 * 
	 */
public:
	StringPool(u32 characterCapacity, u32 maxStringCount)
	    : m_data(characterCapacity), m_meta(maxStringCount) {
		m_data->resize_no_zero_init(m_data->capacity());
	}

	u32 add(std::string_view str) {
		// DevNote: add assert for string too big
		u32 index = addObject_impl(str.size());
		std::copy(str.begin(), str.end(), m_data->begin() + m_meta.buf[index].offset);
		return index;
	}

	std::string_view get(u32 index) const {
		// DevNote: add assert for index out of bound
		Range_impl meta = m_meta.buf[index];
		return std::string_view(
		    m_data->begin() + meta.offset,
		    m_data->begin() + meta.offset + meta.size);
	}

	u32 getSize(u32 index) const { return m_meta.buf[index].size; }




private:
	// basically std::string_view
	struct Range_impl {
		u32 offset, size;
		u32 end() const { return offset + size; }
	};

public:
	// Callback

	// this callback will be called before every string object that is deleted
	// due to eviction
	// since it's called before deletion, the actual meta object and the data
	// it represents will still exist, and be accessable
	// @param u32 the index / handle of the deleting object
	using ObjectDeleteCallback_t = std::function<void(u32)>;

	ObjectDeleteCallback_t m_deleteCb;
	bool m_deleteCbSetted = false;

	void setDeleteCallback(ObjectDeleteCallback_t& cb) {
		m_deleteCb = cb;
		m_deleteCbSetted = true;
	}
	template <std::invocable<u32> Func>
	void setDeleteCallback(Func&& cb) {
		m_deleteCb = ObjectDeleteCallback_t(std::forward<Func>(cb));
		m_deleteCbSetted = true;
	}

private:
	Buffer<char> m_data;
	Buffer<Range_impl> m_meta;
	/**
	 * The m_meta describes the objects / ranges that is stored in m_data
	 * The order of meta objects in m_meta must reflect the actual data order
	 * of them in m_data.
	 */


	// meta buffer house keeping
	/**
	 * The eviction will delete objects from the begin of buffer, and insert
	 * new data at the empty space cleared out
	 * 
	 * The MetaState_impl marks the availability of buffer m_meta.
	 * And the memory availability can be calculated by the metadata stored in
	 * m_meta.
	 * 
	 *  
	 * 
	 * Memory Layout for m_meta: (worse case)
	 * [-- occupied --][-- empty --][-- occupied --][-- empty --]
	 * 
	 * - The first part is the new data that replaced the old, evicted data. it
	 *   always start at begin of buffer, and its end is marked by `frontEnd`,
	 *   which stands for "end of front"
	 * - The second part is the extra meta objects deleted, in the case of the
	 *   memory of one object cannot hold all the data of the incoming string.
	 *   Its end is marked by `backBegin`, which stands for "begin of back"
	 * - The third part is the old data that remained from previous. The first
	 *   entry of it is the oldest string object in the pool. Its begin is
	 *   marked by `backBegin`, which is also the next element to be deleted if
	 *   eviction is requested.
	 * - The fourth part is the uninitialized memory of the buffer, which is
	 *   expected to be small, since because of it cannot fit the incoming
	 *   string, eviction was invoked, created part 1 and 2.
	 * 
	 * [new data | deleted extras | old data | uninitialized]
	 *  ^        ^                ^          ^              ^
	 *  0     frontEnd      backBegin   m_meta.size()  m_meta.capacity()
	 * 
	 * When frontEnd == 0 and backBegin == 0, it indicates that the buffer is
	 * linear, meaning that there was no eviction happend, or all previous
	 * generation data are all evicted - the eviction is resolved
	 */
	struct MetaState_impl {
		u32 frontEnd = 0, backBegin = 0;
	} m_metastate;

	// entry point of the entire evicting logic
	// called by add()
	// @param size the requested size of new data
	// @return the index of the meta object in m_meta. That meta object
	//         represents the free space for the new data to occupy in
	u32 addObject_impl(u32 size) {
		if (size > m_data->capacity()) {
			// DevNote: error - single string overflows the entire buffer
			return InvalidU32;
		}
		/**
		 * Edge case: when everything is empty
		 */
		if (m_meta->empty()) {
			m_meta->push_back(Range_impl{ 0, size });
			m_metastate.frontEnd = 0;
			m_metastate.backBegin = 0;
			return 0;
		}

		u32 index;
		Range_impl object = makeMemoryVacancy_impl(size, index);
		if (index == InvalidU32) { // require makeMetaVacancy_impl
			bool bufferLayoutChanged = false;
			index = makeMetaVacancy_impl(bufferLayoutChanged);
			if (bufferLayoutChanged) {
				// now memory place need to be determind again, since memory
				// layout of m_meta have to match m_data

				// clearing meta object data after deletion for meta integrity
				// preventing the curruption of frontEndIndex calculation in
				// the second makeMemoryVacancy_impl call.
				m_meta.buf[index] = {};

				u32 temp; // we don't care about it's potential index
				// because the index is already determinded
				object = makeMemoryVacancy_impl(size, temp);
			}
		}
		m_meta.buf[index] = object;
		return index;
	}


	// delete a meta object - handles all the memory wise delete operations
	void deleteObject_impl(u32 index) {
		// this function don't actually run any deletion logic, because there
		// is no destructor to call - everything is POD
		if (m_deleteCbSetted)
			m_deleteCb(index);
	}

	// eviction
	/**
	 * There are 2 types of overflows, which both cause a eviction:
	 * (only matters in implementation)
	 * - memory overflow: when incoming string size overflows memory capacity
	 * - meta overflow: when metadata buffer is full
	 * 
	 * Memory eviction is always resolved first, since as if a deletion of
	 * string object is made by memory eviction, the meta overflows always
	 * resolves itself.
	 */



	// eviction resolution
	// - deicde whether invoke a eviction or not


	// @param size requested size of the incoming data
	// @param index the result index of the new data's meta object; if is
	//              InvalidU32 then it need to be resolved by
	//              makeMetaVacancy_impl
	// @return the resolved meta object (a range) of the incoming data
	Range_impl makeMemoryVacancy_impl(u32 size, u32& index) {
		/**
		 * There are 4 cases to this function:
		 * case 1: Linear     - eviction never happend / is fully resolved,
		 *                      everything is linear. But could invoke eviction
		 *                      when reaches the end of capacity
		 * case 2: Interspace - no eviction is invoked, the space lefted by
		 *                      previous eviction is enough for this incoming
		 *                      object
		 * case 3: Pushing    - some objects from m_metastate.backBegin until
		 *                      the end of buffer need to be evicted in order
		 *                      to fit the incoming data
		 * case 4: Wrapping   - the space from m_metastate.frontEnd until the
		 *                      end of buffer is not enough no fit the incoming
		 *                      data; start from the very beginning of the
		 *                      buffer to find enough space for the new data;
		 *                      Everything from the original m_metastate.backEnd
		 *                      to the end of buffer and from the begin of
		 *                      buffer to the new m_metastate.backBegin will all
		 *                      be deleted.
		 */

		// error case: frontEnd > backBegin
		if (m_metastate.frontEnd > m_metastate.backBegin) {
			// DevNote: error
			return Range_impl{ InvalidU32, InvalidU32 };
		}

		// resolve backBegin == end: all previous object deleted; become linear
		if (m_metastate.backBegin >= m_meta->size()) {
			// remove any potential garbage object in interspace
			if (m_metastate.frontEnd != m_metastate.backBegin)
				m_meta->erase(m_meta->begin() + m_metastate.frontEnd, m_meta->end());

			m_metastate.frontEnd = 0;
			m_metastate.backBegin = 0;
		}

		// case 1
		if (m_metastate.frontEnd == 0) {
			u32 currentBufferSize = m_meta->empty() ? 0 : m_meta->back().end();
			if (size > m_data->capacity() - currentBufferSize) {
				// requested size overflows remaining capacity - evict
				index = 0;
				return makeMemoryVacancyBegin_impl(size);
			}
			index = InvalidU32;
			return Range_impl{ currentBufferSize, size };
		}

		u32 frontEndIndex = m_meta.buf[m_metastate.frontEnd - 1].end();
		u32 freeCapacity = m_meta.buf[m_metastate.backBegin].offset -
		                   frontEndIndex;
		// case 2
		if (size <= freeCapacity) {
			// nothing to do
			index = InvalidU32;
			return Range_impl{
				frontEndIndex, size
			};
		}

		// case 3
		if (size <= m_data->capacity() - frontEndIndex) {
			u32 furtherRequiredSize = size - freeCapacity;
			m_metastate.backBegin = makeMemoryVacancyEvict_impl(
			    furtherRequiredSize, m_metastate.backBegin);
			index = InvalidU32;
			return Range_impl{
				frontEndIndex, size
			};
		}

		// case 4
		index = InvalidU32;
		return makeMemoryVacancyBegin_impl(size);
	}
	// this is not a function to solve a case; it's a generic helper
	// After this function returns, there is possibility of the size is not
	// satisfied, but instead had reached the end of m_meta. Depndending on the
	// context, this should be accounted
	// @return the new backBegin / the index of the next object of the last
	//         object deleted
	// @param size the size that is requested
	// @param first the first object to be evicted
	u32 makeMemoryVacancyEvict_impl(u32 size, u32 first) {
		u32 currentSize = 0;
		u32 i = first;
		for (; i < m_meta->size(); i++) {
			deleteObject_impl(i);
			currentSize += m_meta.buf[i].size;
			if (currentSize >= size) {
				i++;
				break;
			}
		}
		return i;
	}
	// find a free space from the beginning
	Range_impl makeMemoryVacancyBegin_impl(u32 size) {
		m_metastate.backBegin = makeMemoryVacancyEvict_impl(size, 0);
		if (m_metastate.backBegin >= m_meta->size()) {
			// absolute everything got deleted
			m_metastate.frontEnd = 0;
			m_metastate.backBegin = 0;
			m_meta->clear();
			m_meta->push_back({});
		} else {
			m_metastate.frontEnd = 1; // there is one object at the very start
		}

		return Range_impl{ 0, size };
	}

	/**
	 * After makeMetaVacancy_impl returns there are 3 possibilities for the
	 * value of it's return value: index of the meta object
	 * 1. Linear, normal push, index = m_meta->size() - 1;
	 * 2. Evicted, normal push, index = old m_metastate.frontEnd
	 * 3. Linear, push but overflows capacity -> evict invoked, index = 0
	 * Notice that in case 3 the memory place need to change, and
	 * makeMemoryVacancy_impl need to be called one more time, since memory
	 * layout of m_meta have to match m_data
	 */

	// context: a new object is inserting
	// call occasion: after memory eviction resolution is over
	// design: lazy, meaning that things don't check the state after they are
	//         done - the next operation checks it.
	// the entry point of meta eviction resolution
	// @param bufferLayoutChanged set to true when buffer order had changed by
	//                            reorder of m_meta; will be used to invoke the
	//                            second makeMemoryVacancy_impl call
	// @return index in m_meta of the object in vacancy
	u32 makeMetaVacancy_impl(bool& bufferLayoutChanged) {
		/**
		 * There are 4 cases (excluding error cases) to this function:
		 * case 1: Linear     - eviction never happend / is fully resolved,
		 *                      everything is linear. But could invoke eviction
		 *                      when reaches the end of capacity
		 * case 2: Full       - frontEnd reached the end of buffer; all
		 *                      previous generation of data is deleted;
		 *                      push_back required; everything resolve back to
		 *                      linear (case 1)
		 * case 3: Interspace - there are more empty slots of meta object
		 *                      remained from previous eviction - just use them
		 * case 4: Adjacent   - the frontEnd had catched up to backBegin there
		 *                      is no more empty slots - meta eviction invokes
		 */

		bufferLayoutChanged = false;

		// error case: frontEnd > backBegin
		if (m_metastate.frontEnd > m_metastate.backBegin) {
			// DevNote: error
			return InvalidU32;
		}

		// resolve backBegin == end: all previous object deleted; become linear
		if (m_metastate.backBegin >= m_meta->size()) {
			// remove any potential garbage object in interspace
			m_meta->erase(m_meta->begin() + m_metastate.frontEnd, m_meta->end());

			m_metastate.frontEnd = 0;
			m_metastate.backBegin = 0;
		}

		// case 1
		if (m_metastate.frontEnd == 0) {
			return makeMetaVacancyLinear_impl(bufferLayoutChanged);
		}

		// case 2
		// unreachable because resolve backBegin == end above already solved it
		// if (m_metastate.frontEnd >= m_meta->size()) {
		// 	m_metastate.frontEnd = 0;
		// 	m_metastate.backBegin = 0;
		// 	return makeMetaVacancyLinear_impl();
		// }

		// case 3
		if (m_metastate.frontEnd < m_metastate.backBegin) {
			m_metastate.frontEnd++;
			return m_metastate.frontEnd - 1;
		}

		// case 4
		if (m_metastate.frontEnd == m_metastate.backBegin) {
			return makeMetaVacancyAdjacent_impl();
		}
		// DevNote: error case - frontEnd bigger then backBegin
		return InvalidU32;
	}
	u32 makeMetaVacancyAdjacent_impl() {
		// DevNote: assertion
		// assert(m_metastate.backBegin < m_meta->size());
		deleteObject_impl(m_metastate.backBegin);
		m_metastate.backBegin++;
		m_metastate.frontEnd++;
		return m_metastate.frontEnd - 1;
	}
	u32 makeMetaVacancyLinear_impl(bool& bufferLayoutChanged) {
		if (m_meta->size() == m_meta->capacity()) {
			// meta overflow - meta eviction invokes; then switch to adjacent
			// - directly use adjacent here because adjacent already included
			//   eviction.
			bufferLayoutChanged = true; // declare buffer layout change, since
			// m_meta wrapping back to start means that memory place need to be
			// re-determinded from the start as well
			// only set here but not makeMetaVacancyAdjacent_impl because in
			// the general makeMetaVacancyAdjacent_impl case the memory layout
			// is not garanteed to change
			return makeMetaVacancyAdjacent_impl();
		}
		m_meta->push_back({});
		return m_meta->size() - 1;
	}
};

class LineBuffer {
public:
private:
};

/**
 * Link StringPool and TextBuffer together, for the overflow of StringPool
 * can take affect at TextBuffer
 */
class LineBufferManager {
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
	// clang-format off
	inline static constexpr const i8 keyShiftTable[] = {
		i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1},
		i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1},
		i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1},
		i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1}, i8{-1},
		'"', // ''' Apostrophe


		
	};
	// clang-format on

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