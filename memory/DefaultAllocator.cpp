#include "DefaultAllocator.h"
#include "..\utils\Log.h"

namespace ds {

	DefaultAllocator* gDefaultMemory;

// Given a pointer to the header, returns a pointer to the data that follows it.
inline void *data_pointer(Header *header, uint32 align) {
	void *p = header + 1;
	return memory::align_forward(p, align);
}

// Given a pointer to the data, returns a pointer to the header before it.
inline Header *header(void *data) {
	uint32 *p = (uint32 *)data;
	while (p[-1] == HEADER_PAD_VALUE)
		--p;
	return (Header *)p - 1;
}

// Stores the size in the header and pads with HEADER_PAD_VALUE up to the
// data pointer.
inline void fill(Header *header, void *data, uint32 size) {
	header->size = size;
	uint32 *p = (uint32 *)(header + 1);
	while (p < data)
		*p++ = HEADER_PAD_VALUE;
}

DefaultAllocator::DefaultAllocator(uint32 size) : _capacity(size) {
	_buffer = (char*)malloc(sizeof(char) * size);
}


DefaultAllocator::~DefaultAllocator() {
	//LOG << "Removing DefaultAllocator - allocations: " << total_allocated();
	//assert(total_allocated() == 0);
	if (total_allocated() > 0) {
		LOG << "--------------- Memory leaks ----------------------------";
		for (uint32 i = 0; i < _headers.size(); ++i) {
			const Header& h = _headers[i];
			if (h.used) {
				LOG << i << " size: " << h.size << " index: " << h.index << " used: " << h.used;
			}
		}
	}
	delete[] _buffer;
}

void* DefaultAllocator::allocate(uint32 size, uint32 align) {
	//LOG << "Allocating size " << size << " align " << align;
	uint32 s = size + align;
	int idx = -1;
	for (uint32 i = 0; i < _headers.size(); ++i) {
		const Header& h = _headers[i];
		if (!h.used && h.size >= s && idx == -1) {
			idx = i;
		}
	}
	if (idx == -1) {
		//LOG << "creating new header";
		Header h;
		h.used = true;
		h.size = s;
		h.originalSize = s;
		h.index = 0;
		if (!_headers.empty()) {
			const Header& last = _headers.back();
			h.index = last.index + last.size;
		}
		_headers.push_back(h);
		void* p = _buffer + h.index;
		return p;
	}
	else {
		//LOG << "using header at: " << idx;
		Header& h = _headers[idx];
		h.used = true;
		void* p = _buffer + h.index;
		return p;
	}
}

void DefaultAllocator::deallocate(void *p) {
	if (!p)
		return;
	int d = (int)((char*)p - _buffer);
	for (uint32 i = 0; i < _headers.size(); ++i) {
		Header& h = _headers[i];
		if (h.index == d) {
			//LOG << "dealloc - found header at: " << i;
			h.used = false;
		}
	}
}

uint32 DefaultAllocator::allocated_size(void *p) {
	int d = (int)((char*)p - _buffer);
	for (uint32 i = 0; i < _headers.size(); ++i) {
		const Header& h = _headers[i];
		if (h.index == d && h.used) {
			return h.size;
		}
	}
	return 0;
}

uint32 DefaultAllocator::total_allocated() {
	uint32 total = 0;
	for (uint32 i = 0; i < _headers.size(); ++i) {
		const Header& h = _headers[i];
		if (h.used) {
			total += h.size;
		}
	}
	return total;
}

void DefaultAllocator::debug() {
	for (uint32 i = 0; i < _headers.size(); ++i) {
		const Header& h = _headers[i];
		LOG << i << " size: " << h.size << " (" << h.originalSize << ") index : " << h.index << " used : " << h.used;
	}
}

}