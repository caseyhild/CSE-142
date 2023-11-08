#include<cstdint>
#include"omp.h"
#include"cfiddle.hpp"
#include"util.hpp"

uint64_t  canary_size = 500000000;
uint64_t histogram[256];

// __attribute__ ((optimize("O3")))

extern "C"
void  _canary(uint threads) {
	uint64_t arg1 = 1024;
	uint64_t * data = new uint64_t[canary_size];
	uint64_t histogram[256];
	flush_caches();
	start_measurement("canary");
	omp_set_num_threads(threads);
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}

#pragma omp parallel for
	for(uint64_t ii = 0; ii < canary_size; ii+=arg1) {
		uint64_t my_histogram[256];
		for(int i =0; i < 256;i++) {
			my_histogram[i] = 0;
		}
		for(uint64_t i = ii; i < canary_size && i < ii + arg1; i++) {
			
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


	delete data;
	end_measurement();
}
