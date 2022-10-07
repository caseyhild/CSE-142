#include <cstdlib>
#include <unistd.h>
#include <unordered_set>
#include<algorithm>
#include<cstdint>
#include"cfiddle.hpp"

#define CLINK extern "C"
#define OPT(a) __attribute__(a)

extern "C" void init(uint64_t * array, uint64_t size) {
	for(uint i= 0 ; i < size; i++) {
		array[i] = 0;
	}
}

extern "C" void baseline_int(uint64_t * array, uint64_t size) {
	for (uint j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}
	}
}

extern "C" void __attribute__ ((optimize(4))) baseline_int_O4 (uint64_t * array, uint64_t size) {
	for (uint j = 0; j < 3; j++) {
		for(uint i= 1 ; i < size; i++) {
			array[i] += i/(1+j)+array[i - 1];
		}

	}
}

volatile int ROW_SIZE = 1024;
extern "C" void __attribute__ ((optimize(4))) matrix_column_major(uint64_t * array, uint64_t size) {


	for (int k = 0; k < ROW_SIZE; k++) {
		for(uint i= 0 ; i < size/ROW_SIZE; i++) {
			array[i*ROW_SIZE + k] += 1.0; // This Line
		}
	}

}


extern "C" void __attribute__ ((optimize(4))) matrix_row_major(uint64_t * array, uint64_t size) {

	for(uint i= 0; i < size/ROW_SIZE; i++) {
		for (int k = 0; k < ROW_SIZE; k++) {
			array[i*ROW_SIZE + k] += 1.0; // This Line
		}
	}
}
