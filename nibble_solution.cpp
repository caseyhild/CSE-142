
#include <vector>
#include <iostream>
#include "cfiddle.hpp"
#include "nibble.hpp"

extern "C" uint64_t nibble_search(uint8_t query, const std::vector<uint16_t> & targets) {
    uint64_t count = 0;

    for(auto & i: targets) {
        for(int b = 0; b < 16 - 4; b++) {
            uint8_t extracted = ((i >> b) & (0xf));
            if (extracted == query) {
                count+=1;
            }
        }
    }
    return count;
}

// Cfiddle-signature=58b10cd517e64e29d69e3a55b1320442