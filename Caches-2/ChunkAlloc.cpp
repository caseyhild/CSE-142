#include <stdlib.h>
#include<iostream>
#include <string.h>
#include "ChunkAlloc.hpp"
#include <sys/mman.h>

static size_t allocated_chunks = 0;

void init_chunk() {
	// This used to do stuff for Moneta. Now it's empty.
}

void * alloc_chunk() { // allocate CHUNK_SIZE bytes of memory by asking the operating system for it.
	
	// this is how malloc gets it's memory from the kernel.
	// mmap() can do many things.  In this case, it just asks the kernel to
	// give us some pages of memory.  They are guaranteed to contain zeros.
	void * r = mmap(NULL, CHUNK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0); 
	if (r == MAP_FAILED) { 
		std::cerr << "alloc_chunk() failed. This often means you've allocated too many chunks. Exiting: " << strerror(errno) << "\n";
		exit(1);
	}
	allocated_chunks++; // This is just statistics tracking
	return r;
}

void free_chunk(void*p) { // Return the chunk to the OS.  After this, accesses to the addresses in the chunk will result in SEGFAULT
	int r = munmap(p, CHUNK_SIZE);
	if (r != 0) {
		std::cerr << "free_chunk() failed. exiting: " << strerror(errno) << "\n";
		exit(1);
	}
	allocated_chunks--;
}

size_t get_allocated_chunks() {
	return allocated_chunks;
}
