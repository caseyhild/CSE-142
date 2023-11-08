#include<iostream>
#include "pin_tags.h"
#include<vector>
#include "archlab.hpp"
//#define SIZE 2
void simple(int SIZE) {
	int A[SIZE][SIZE];
	int B[SIZE][SIZE];
	int C[SIZE][SIZE];
	//int c = 0;
	std::stringstream ss;
	
	ss << "Size-" << SIZE;
	
	NEW_TRACE(ss.str().c_str());
	START_TRACE();
	for(int i = 0; i < SIZE; i++) {
		for(int j = 0; j < SIZE; j++) {
			for(int k = 0; k < SIZE; k++) {
				//std::cout << c++ << ": C[" << i << ", " << j << "] A[" << i << ", " << k <<"] B[" << k << ", " <<j << "]\n";
				//std::cout << c++ << ": (i, j, k) = (" << i << ", " << j << ", " << k << ")\n";
				C[i][j]	+= A[i][k] * B[k][j];
			}
		}
	}


}
//#undef SIZE
// #define SIZE 16
							//std::cout << "C[" << i << ", " << j << "] A[" << i << ", " << k <<"] B[" << k << ", " <<j << "]\n";

void tiled( int SIZE) {
	int A[SIZE][SIZE];
	int B[SIZE][SIZE];
	int C[SIZE][SIZE];

	for(int ii = 0; ii < SIZE; ii += SIZE) {
		for(int jj = 0; jj < SIZE; jj += SIZE) {
			for(int kk = 0; kk < SIZE; kk += SIZE) {

				for(int i = ii; i < (ii + SIZE); i++) {
					for(int j = jj; j < (jj + SIZE); j++) {
						for(int k = kk; k < (kk + SIZE); k++) {
							C[i][j]	+= A[i][k] * B[k][j];
						}
					}
				}
				
			}
		}
	}
}

int main(int argc, char *argv[]) {
	archlab_parse_cmd_line(&argc, argv);
	std::vector<int> sizes ={ 16, 32, 36, 40, 44, 48, 52, 56, 60, 64, 70, 74, 78,82,86,90,94,98,102,106,110,114,118, 122, 124,128};
	for(auto size: sizes) {
		
		pristine_machine();
		theDataCollector->disable_prefetcher();
		{
			ArchLabTimer timer;

			timer.attr("size", size);
			timer.go();
			//			for(int i = 0; i < 1000; i++)
				simple(size);
		}
	}

	archlab_write_stats();	
}

