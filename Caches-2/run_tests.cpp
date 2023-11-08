#define DEBUG_OUTPUT "output/"

#include <iostream>
#include "gtest/gtest.h"
#include <sstream>
#include "AlignedAllocator.hpp"
#include "ReferenceAllocator.hpp"

#define HIDDEN_SEEDS

template<int SIZE>
struct Thing{
	uint8_t stuff[SIZE];
};



namespace Tests {


	template<typename Alloc>
	void test_alloc(uint64_t alloc_count, uint64_t dealloc_count, uint64_t dealloc_chunk, uint64_t seed) {
		auto alloc = new Alloc;
		std::set<typename Alloc::ItemType*> allocated;			
									
		for(uint i  = 0; i < alloc_count; i++) {			
			auto n = alloc->alloc();			
			EXPECT_EQ(allocated.find(n), allocated.end()) << "Allocator returned " << n << ", but it's already allocated and hasn't been freed."; 
			EXPECT_EQ((reinterpret_cast<uintptr_t>(n) %  Alloc::Alignment), 0ul) << "Incorrect alignment.  Expected " << Alloc::Alignment << "-byte aligned got " << n; 
			for(uint j = 0; j < sizeof(typename Alloc::ItemType); j++) {			
				EXPECT_EQ(n->stuff[j], 0) << "Non-zeroed data";	       
			}						
			allocated.insert(n);				
		}							
									
		for(uint k = 0; k < dealloc_count; k+=dealloc_chunk) {			
			for (uint j =0 ; j < dealloc_count - k; j++) {		
				auto n = alloc->alloc();		
				EXPECT_EQ(allocated.find(n), allocated.end()) << "Allocator returned " << n << ", but it's already allocated and hasn't been freed."; 
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % Alloc::Alignment), 0ul)<< "Incorrect alignment.  Expected " << Alloc::Alignment << "-byte aligned got " << n; 
				for(uint j = 0; j < sizeof(typename Alloc::ItemType); j++)		
					EXPECT_EQ(n->stuff[j], 0) << "Non-zeroed data";		
				allocated.insert(n);			
			}						
			for (uint j = 0; j < k ; j++) {			
				alloc->free(*allocated.begin());	
				allocated.erase(*allocated.begin());
			}
		}
									
		for(uint i  = 0; i < alloc_count; i++) {			
			alloc->free(*allocated.begin());		
			allocated.erase(*allocated.begin());		
		}							
									
		delete alloc;						
	}

	using testing::Types;
	
	typedef Types<ReferenceAllocator<Thing<4>,16>,  
		      ReferenceAllocator<Thing<16>,16>,  
		      ReferenceAllocator<Thing<3>,16>, 
		      ReferenceAllocator<Thing<3>,8>,
		      ReferenceAllocator<Thing<27>,32>,
		      ReferenceAllocator<Thing<101>,32>,
		      AlignedAllocator<Thing<4>,16>,  
		      AlignedAllocator<Thing<16>,16>,  
		      AlignedAllocator<Thing<3>,16>, 
		      AlignedAllocator<Thing<3>,8>,
		      AlignedAllocator<Thing<27>,32>,
		      AlignedAllocator<Thing<101>,32>
		      > LongAllocatorImplementations;

	template <class T>
	class AllocatorTest : public testing::Test {};
	
	TYPED_TEST_SUITE(AllocatorTest, LongAllocatorImplementations);

	TYPED_TEST(AllocatorTest, alloc_test2) {
		test_alloc<TypeParam>   (100, 50, 10, 1);
		test_alloc<TypeParam>   (10000, 5000, 100, 2);
		test_alloc<TypeParam>   (10000, 8000, 400, 3);
	}
	
	
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

#include"hidden_tests.cpp"

	typedef Types<ReferenceAllocator<Thing<4>,16>,  
		      ReferenceAllocator<Thing<1025>,16>,  
		      AlignedAllocator<Thing<4>,16>,
		      AlignedAllocator<Thing<1025>,16>
		      > ShortAllocatorImplementations;

	template <class T>
	class AllocatorTestSanity : public testing::Test {};
	
	TYPED_TEST_SUITE(AllocatorTestSanity, ShortAllocatorImplementations);

	TYPED_TEST(AllocatorTestSanity, recycle_test) {
		{
			std::vector<uint64_t> seeds = {
				0xDEADBEEF,
				0xABAD1DEA
				HIDDEN_SEEDS
			};

			for(auto s : seeds) {
				TypeParam alloc;
				uint count = (fast_rand(&s) % 1000) + 1000;
				uint iterations = (fast_rand(&s) % 100) + 50;
				// This call and the ones below will always allocate
				// and deallocate the same number of objects, so no new
				// chunks should be needed.
				exercise(&alloc, count, iterations, s, true); 
				int allocated_chunks_start = get_allocated_chunks();
				for(int i = 0; i < 10; i++) {
					exercise(&alloc, count, iterations, s, true);
					int allocated_chunks_end = get_allocated_chunks();
					EXPECT_GE(allocated_chunks_start, allocated_chunks_end) << "It looks like you are not recycling properly";
				}
			}
		}
	}

	
	TYPED_TEST(AllocatorTestSanity, alignment_test_small) {
		{
			TypeParam alloc;
			for(int i=  0; i < 10; i++) {
				auto n = alloc.alloc();
				EXPECT_EQ((reinterpret_cast<uintptr_t>(n) % TypeParam::Alignment), 0ul) << "While allocatating an oject of size "
													<< sizeof(typename TypeParam::ItemType) << ", aligned to "
													<< TypeParam::Alignment << " byte boundaries, the allocator provide misaligned memory.";
			}
		}
		EXPECT_EQ(get_allocated_chunks(), 0ul) << "The allocator failed to chunk_free() some chunks";
	}

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
