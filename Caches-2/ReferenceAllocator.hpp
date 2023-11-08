#include <stdlib.h>
#include <iostream>
#include <set>
#include <cstring>
#include "cfiddle.hpp"
#include"ChunkAlloc.hpp"

template<
	class T,          // This is the type we are allocating.  You can assume this is less than or equal to 4kB
	size_t ALIGNMENT  // The alignment at which we much allocate the objects.  You can assume this is less than or equal to 4kB
	> 
class ReferenceAllocator {
	std::set<T*> chunks; // We store everything we allocated so we can clean up in the destructor.
public:
 	typedef T ItemType; // This will make T available as ReferenceAllocator::ItemType 
	static const size_t Alignment = ALIGNMENT;  // Likewise, we can access the alignment as ReferenceAllocator::Alignment
	
	ReferenceAllocator() {}
	
        T * alloc() {
		void* p  = NULL;
		// this system call can allocate aribitrary-sized and aligned
		// objects.  Since it can handle any size, it's more general.
		int r =  posix_memalign(&p, ALIGNMENT, sizeof(T));
		if (r != 0) { 
			std::cerr << "posix_memalign() failed.  Exiting: " << strerror(r) << "(" << r << ")\n";
			exit(1);
		}
		uint8_t * t = reinterpret_cast<uint8_t*>(p); // alloc_chunk provides void*, but we can assign to void.  So cast...
		for(uint i= 0; i < sizeof(T); i++) {
			t[i] = 0; // and set to zero.
		}
		T* c = reinterpret_cast<T*>(p); // cast to the type we allocate.
		new (c) T; // This is the "in place" new operator.  It constructs an object at a given location.
		chunks.insert(c); // record it so we can delete it later.
		return c;
	}
	
	void free(T * p) {
		std::free(reinterpret_cast<void*>(p)); // Return the memory
		chunks.erase(p); // note that it's no longer allocated.
	}

	~ReferenceAllocator() {
		for(auto & p: chunks) { 
			std::free(reinterpret_cast<void*>(p)); // Return everything that still allocated.
		}
	}
};

template<class T, size_t ALIGNMENT> 
const size_t ReferenceAllocator<T, ALIGNMENT>::Alignment;
