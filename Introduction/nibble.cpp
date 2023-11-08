#include "cfiddle.hpp"
#include<iostream>
#include <vector>
#include<cstdlib>


extern "C" uint64_t nibble_search(uint8_t query, const std::vector<uint16_t> & targets);

void rand_init(std::vector<uint16_t> & v, uint64_t & seed) {
	for(auto &i : v) {
		i = fast_rand(&seed);
	}
}

extern "C" uint64_t run_nibble(uint64_t target_count, uint64_t seed) {

	// Get the secret seed for final testing.
	char * secret_seed = getenv("CSE142L_SEED");
	if (secret_seed) {
		seed = atoll(secret_seed);
		unsetenv("CSE142L_SEED");
	}

	// Scramble the seed some.
	int scramble_count = fast_rand(&seed) % 100;
	for(int i = 0; i < scramble_count; i++) {
		fast_rand(&seed);
	}
    
	std::vector<uint16_t> targets(target_count);
    
	rand_init(targets, seed);
	uint8_t query = fast_rand(&seed) & 0xf;

	start_measurement();
	auto answer = nibble_search(query, targets);
	end_measurement();
    
	return answer;
}
