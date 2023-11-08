#include"cfiddle.hpp"
#include"omp.h"
#include<iostream>
#include<cstdint>
#include<thread>
#include<mutex>
#include"threads.hpp"
#include"pthread.h"
#include"util.hpp"

//START_UNTHREADED
volatile uint64_t histogram[256];
extern "C"
void __attribute__((noinline)) unthreaded_histogram(uint64_t* data, uint64_t size)
{
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			histogram[b]++;
		}
	}
}

extern "C"
void run_unthreaded_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	start_measurement();
	unthreaded_histogram(data, size);
	end_measurement();
	delete [] data;
}
//END_UNTHREADED


//START_THREADED
std::mutex lock;

extern "C"
void __attribute__((noinline)) threaded_histogram(uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			lock.lock(); // we have to take a lock when we access shared data.
			histogram[b]++;
			lock.unlock();
		}
	}
}

#define force_cast(X) reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(X))
extern "C"
void run_threaded_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	start_measurement();
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(threaded_histogram, &data[chunk_length * i], chunk_length);
	}

	threaded_histogram(&data[chunk_length * (thread_count - 1)],
			   size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	end_measurement();
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete [] data;

}
//END_THREADED


//START_FINE
std::mutex *fine_locks[256];

extern "C"
void __attribute__((noinline)) fine_locks_histogram(uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			fine_locks[b]->lock(); // we have to take a lock when we access shared data.
			histogram[b]++;
			fine_locks[b]->unlock(); 
		}
	}
}

extern "C"
void run_fine_locks_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
		fine_locks[i] = new std::mutex();
	}
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	start_measurement();	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(fine_locks_histogram, &data[chunk_length * i], chunk_length);
	}

	fine_locks_histogram(&data[chunk_length * (thread_count - 1)],
			   size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	end_measurement();
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	
	delete threads;

	for(int i =0; i < 256;i++) {
		delete fine_locks[i];
	}
	delete [] data;
}
//END_FINE


//START_PRIVATE
extern "C"
void __attribute__((noinline)) private_histogram(int id, int thread_count, uint64_t *buckets, uint64_t * data, uint64_t size) {

	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			buckets[b*thread_count + id]++;
		}
	}	

}

extern "C"
void run_private_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	uint64_t * private_buckets = new uint64_t[thread_count * 256]();
	
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down
	flush_caches();
	disable_prefetcher();
	start_measurement();	
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(private_histogram,
					     i,
					     thread_count,
					     private_buckets,
					     &data[chunk_length * i],
					     chunk_length);
	}

	private_histogram(thread_count - 1,
			  thread_count,
			  private_buckets,
			  &data[chunk_length * (thread_count - 1)],
			  size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	end_measurement();	

	for(int b = 0; b < 256; b++) {
		for(unsigned int id = 0; id < thread_count; id++) {
			histogram[b] += private_buckets[b*thread_count + id]++;
		}
	}

	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete private_buckets;
	delete [] data;

}
//END_PRIVATE


//START_PRIVATE2
extern "C"
void __attribute__((noinline)) private2_histogram(int id, int thread_count, uint64_t *buckets, uint64_t * data, uint64_t size) {
	
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			buckets[id*256 + b]++;
		}
	}
}

extern "C"
void run_private2_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	uint64_t * private_buckets = new uint64_t[thread_count * 256]();
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	
	std::thread **threads = new std::thread*[thread_count];
	int chunk_length = size/thread_count; // chunk_length might be rounded down

	start_measurement();	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(private2_histogram,
					     i,
					     thread_count,
					     private_buckets,
					     &data[chunk_length * i],
					     chunk_length);
	}

	private2_histogram(thread_count - 1,
			  thread_count,
			  private_buckets,
			  &data[chunk_length * (thread_count - 1)],
			  size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}
	end_measurement();	


	for(unsigned int id = 0; id < thread_count; id++) {
		for(int b = 0; b < 256; b++) {
			histogram[b] += private_buckets[id*256 + b]++;
		}
	}

	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete private_buckets;
	delete [] data;
}
//END_PRIVATE2



//START_OPENMP

extern "C"
void run_openmp_histogram(uint64_t thread_count, uint64_t size) {
	uint64_t * data = new uint64_t[size]; 
	omp_set_num_threads(thread_count);

	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}
	start_measurement();	
#pragma omp parallel for 
	for(uint64_t i = 0; i < size; i++) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
#pragma omp critical 
			histogram[b]++;
		}
	}
	end_measurement();	
	delete [] data;
}
//END_OPENMP


//START_OPENMP_PRIVATE

extern "C"
void run_openmp_private_histogram(uint64_t thread_count, uint64_t size, uint64_t chunk_size) {
	uint64_t * data = new uint64_t[size]; 

	omp_set_num_threads(thread_count);
							
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}

	start_measurement();	
#pragma omp parallel for
	for(uint64_t ii = 0; ii < size; ii+=chunk_size) {
		uint64_t my_histogram[256];
		for(int i =0; i < 256;i++) {
			my_histogram[i] = 0;
		}
		for(uint64_t i = ii; i < size && i < ii + chunk_size; i++) {
			
			for(int k = 0; k < 64; k+=8) {
				uint8_t b = (data[i] >> k)& 0xff;
				my_histogram[b]++;
			}
		}
#pragma omp critical 
		for(int i =0; i < 256;i++) {
			histogram[i] += my_histogram[i];
		}
	}
	end_measurement();	
	delete [] data;
}
//END_OPENMP_PRIVATE

