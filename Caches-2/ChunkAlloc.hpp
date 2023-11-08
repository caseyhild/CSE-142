#define CHUNK_SIZE (128*1024)

extern void init_chunk(); // Set things up.
extern void * alloc_chunk(); // Allocate CHUNK_SIZE of memory
extern void free_chunk(void*p); // Free CHUNK_SIZE of memory
extern size_t get_allocated_chunks(); // Query how many chunks are currently allocated.
