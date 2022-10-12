
#include <vector>
#include <iostream>
#include "cfiddle.hpp"
#include "nibble.hpp"

extern "C" uint64_t nibble_search(uint8_t query, const std::vector<uint16_t> & targets) {
    uint64_t count = 0;

    for(auto & i: targets) {
        count += (((i) & (0xf)) == query) + (((i >> 1) & (0xf)) == query) + (((i >> 2) & (0xf)) == query) + (((i >> 3) & (0xf)) == query)
            + (((i >> 4) & (0xf)) == query) + (((i >> 5) & (0xf)) == query) + (((i >> 6) & (0xf)) == query) + (((i >> 7) & (0xf)) == query)
            + (((i >> 8) & (0xf)) == query) + (((i >> 9) & (0xf)) == query) + (((i >> 10) & (0xf)) == query) + (((i >> 11) & (0xf)) == query);
    }
    return count;
}

// Cfiddle-signature=58b10cd517e64e29d69e3a55b1320442
