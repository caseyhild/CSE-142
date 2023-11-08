#include"cfiddle.hpp"

uint64_t  canary_size = 500000000;
uint64_t histogram[256];

extern "C"
void  __attribute__ ((optimize("O3"))) _canary(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	uint64_t * data = new uint64_t[canary_size];


	start_measurement("canary");
	for(int i =0; i < 256;i++) {
		histogram[i] = 0;
	}

	for(uint64_t i = 0; i < canary_size; i+=1) {
		for(int k = 0; k < 64; k+=8) {
			uint8_t b = (data[i] >> k)& 0xff;
			histogram[b]++;
		}
	}
	end_measurement();

	delete data;
}
