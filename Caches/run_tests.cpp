//#define INCLUDE_TESTS
#define DEBUG_OUTPUT "output/"

#include <iostream>
#include "gtest/gtest.h"
#include <opt_cnn.hpp>
#include <sstream>
#include "FastAllocator.hpp"
#include "Allocator.hpp"
#define ALLOC_TEST_(ALLOC, SIZE, ALIGNMENT, DASH)			\
	TEST_F(OptimizationTests, alignment_test_##ALLOC##DASH##SIZE##DASH##ALIGNMENT) {	\
		auto alloc = new ALLOC<Thing<SIZE>, ALIGNMENT>; \
		std::set<Thing<SIZE>*> allocated;			\
									\
		for(int i  = 0; i < 10000; i++) {			\
			auto n = alloc->alloc();			\
<<<<<<< Updated upstream
			EXPECT_EQ(allocated.find(n), allocated.end());	\
			EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % ALIGNMENT), 0ul); \
=======
			EXPECT_EQ(allocated.find(n), allocated.end()) << "Allocator returned " << n << ", but it's already allocated and hasn't been freed."; \
			EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % ALIGNMENT), 0ul) << "Incorrect alignment.  Expected " << ALIGNMENT << "-byte aligned got " << n; \
			for(int j = 0; j < SIZE; j++) {			\
				EXPECT_EQ(n->stuff[j], 0) << "Non-zeroed data";		\
			}						\
>>>>>>> Stashed changes
			allocated.insert(n);				\
		}							\
									\
		for(int k = 0; k < 5000; k+=100) {			\
			for (int j =0 ; j < 5000 - k; j++) {		\
				auto n = alloc->alloc();		\
<<<<<<< Updated upstream
				EXPECT_EQ(allocated.find(n), allocated.end()); \
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % ALIGNMENT), 0ul); \
=======
				EXPECT_EQ(allocated.find(n), allocated.end()) << "Allocator returned " << n << ", but it's already allocated and hasn't been freed."; \
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % ALIGNMENT), 0ul)<< "Incorrect alignment.  Expected " << ALIGNMENT << "-byte aligned got " << n; \
				for(int j = 0; j < SIZE; j++)		\
					EXPECT_EQ(n->stuff[j], 0) << "Non-zeroed data";		\
>>>>>>> Stashed changes
				allocated.insert(n);			\
			}						\
			for (int j = 0; j < k ; j++) {			\
				alloc->free(*allocated.begin());	\
				allocated.erase(*allocated.begin());	\
			}						\
		}							\
									\
		for(int i  = 0; i < 10000; i++) {			\
			alloc->free(*allocated.begin());		\
			allocated.erase(*allocated.begin());		\
		}							\
									\
		delete alloc;						\
	}	
#define ALLOC_TEST(ALLOC, SIZE, ALIGNMENT) ALLOC_TEST_(ALLOC, SIZE, ALIGNMENT, _)

namespace Tests {

      	
	class OptimizationTests :  public ::testing::Test {
	};

	ALLOC_TEST(ReferenceAllocator, 4, 16);
	ALLOC_TEST(ReferenceAllocator, 16, 16);
	ALLOC_TEST(ReferenceAllocator, 3, 16);
	ALLOC_TEST(ReferenceAllocator, 3, 8);
	ALLOC_TEST(ReferenceAllocator, 27, 32);
	ALLOC_TEST(ReferenceAllocator, 101, 1024);
			  
	ALLOC_TEST(FastAllocator, 4, 16);
	ALLOC_TEST(FastAllocator, 16, 16);
	ALLOC_TEST(FastAllocator, 3, 16);
	ALLOC_TEST(FastAllocator, 3, 8);
	ALLOC_TEST(FastAllocator, 27, 32);
	ALLOC_TEST(FastAllocator, 101, 1024);
			  
<<<<<<< Updated upstream
=======
	BIG_ALLOC_TEST(SolutionAllocator, 4, 16);
	BIG_ALLOC_TEST(SolutionAllocator, 16, 16);
	BIG_ALLOC_TEST(SolutionAllocator, 3, 16);
	BIG_ALLOC_TEST(SolutionAllocator, 3, 8);
	BIG_ALLOC_TEST(SolutionAllocator, 27, 32);
	BIG_ALLOC_TEST(SolutionAllocator, 101, 1024);
#endif

	template<class Allocator>
	void exercise(Allocator * allocator, size_t count, int iterations, uint64_t seed, bool cleanup = false) {
		
		std::vector<typename Allocator::ItemType *> items(count);

		
		for(unsigned int i = 0; i < count; i++)
			items[i] = NULL;
		
		for(int i = 0; i < iterations; i++) {
			for(unsigned int j = 0; j < count; j++) {
				if (items[j] == NULL) {
				items[j] = allocator->alloc();
				}
			}
			for(unsigned int j = 0; j < count; j++) {
				fast_rand(&seed);
				if (seed & 0x3) {
					allocator->free(items[j]);
					items[j] = NULL;
				}
			}
		}
		if (cleanup) {
			for(unsigned int j = 0; j < count; j++) {
				if (items[j]) {
					allocator->free(items[j]);
					items[j] = NULL;
				}
			}
		}
	}


	TEST_F(OptimizationTests, recycle_test_reference) {
		{
			std::vector<uint64_t> seeds = {0x325235,
						       0xDEADBEEF,
						       0xABAD1DEA,
						       0x1,
						       0x422A235A};

			for(auto s : seeds) {
				std::cout << "Running exercise with seed " << s  << "\n";
				auto alloc = new ReferenceAllocator<Thing<10>, 16>;
				// THis call and the ones below will always allocate
				// and deallocate the same number of objects, so no new
				// chunks should be needed.
				exercise(alloc, 1000, 50, s, true); 
				int allocated_chunks_start = get_allocated_chunks();
				for(int i = 0; i < 10; i++) {
					exercise(alloc, 1000, 50, s, true);
					int allocated_chunks_end = get_allocated_chunks();
					EXPECT_EQ(allocated_chunks_start, allocated_chunks_end) << "It looks like you are not recycling properly"; ;
				}
				delete alloc;
			}
		}
	}

	
	TEST_F(OptimizationTests, alignment_test_small_reference) {
		{
			auto alloc = new ReferenceAllocator<Thing<10>, 16>;
			for(int i=  0; i < 10; i++) {
				auto n = alloc->alloc();
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % 16), 0ul) << "While allocatating an oject of size 10, aligned to 16 byte boundaries, the allocator provide misaligned memory.";
			}
			delete alloc;
		}
		EXPECT_EQ(get_allocated_chunks(), 0ul) << "The allocator failed to chunk_free() some chunks";
		
		{
			auto alloc = new ReferenceAllocator<Thing<1022>, 128>;
			for(int i=  0; i < 10; i++) {
				auto n = alloc->alloc();
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % 128), 0ul) << "While allocatating an oject of size 1022, aligned to 128 byte boundaries, the allocator provide misaligned memory.";
			}
			delete alloc;
		}
		EXPECT_EQ(get_allocated_chunks(), 0ul) << "The allocator failed to chunk_free() some chunks";

	}

	TEST_F(OptimizationTests, recycle_test_solution) {
		{
			std::vector<uint64_t> seeds = {0x325235,
						       0xDEADBEEF,
						       0xABAD1DEA,
						       0x1,
						       0x422A235A};

			for(auto s : seeds) {
				std::cout << "Running exercise with seed " << s << "\n";
				auto alloc = new SolutionAllocator<Thing<10>, 16>;
				// THis call and the ones below will always allocate
				// and deallocate the same number of objects, so no new
				// chunks should be needed.
				exercise(alloc, 1000, 50, s, true); 
				int allocated_chunks_start = get_allocated_chunks();
				for(int i = 0; i < 10; i++) {
					exercise(alloc, 1000, 50, s, true);
					int allocated_chunks_end = get_allocated_chunks();
					EXPECT_EQ(allocated_chunks_start, allocated_chunks_end) << "It looks like you are not recycling properly"; ;
				}
				delete alloc;
			}
		}
	}

	TEST_F(OptimizationTests, alignment_test_small_solution) {
		{
			auto alloc = new SolutionAllocator<Thing<10>, 16>;
			for(int i=  0; i < 10; i++) {
				auto n = alloc->alloc();
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % 16), 0ul) << "While allocatating an oject of size 10, aligned to 16 byte boundaries, the allocator provide misaligned memory.";
			}
			delete alloc;
		}
		EXPECT_EQ(get_allocated_chunks(), 0ul) << "The allocator failed to chunk_free() some chunks";
		
		{
			auto alloc = new SolutionAllocator<Thing<1022>, 128>;
			for(int i=  0; i < 10; i++) {
				auto n = alloc->alloc();
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % 128), 0ul) << "While allocatating an oject of size 1022, aligned to 128 byte boundaries, the allocator provide misaligned memory.";
			}
			delete alloc;
		}
		EXPECT_EQ(get_allocated_chunks(), 0ul) << "The allocator failed to chunk_free() some chunks";
	}

	
>>>>>>> Stashed changes
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
