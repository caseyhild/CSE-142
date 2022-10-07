#include "gtest/gtest.h"
#include <algorithm>
#include "fastrand.h"
#include <random>

extern "C" uint64_t run_nibble(uint64_t target_count, uint64_t seed);
extern "C" uint64_t nibble_search(uint8_t query,  const std::vector<uint16_t> & targets);

namespace Tests {

	class NibbleTestFixtureSmall: public ::testing::TestWithParam<std::tuple<uint16_t, uint8_t, uint64_t>> {
        };


	TEST_P(NibbleTestFixtureSmall, simple_tests) {
		uint16_t target = std::get<0>(GetParam());
		uint8_t  query  = std::get<1>(GetParam());
		uint64_t answer = std::get<2>(GetParam());
		
		std::vector<uint16_t> v{ target };
		std::cout << v.size() << "\n";
		auto computed = nibble_search(query, v);
		ASSERT_EQ(computed, answer) << "nibble_search(" << (int)query << ", [" << target << "]) == " << answer << " but your code produced " << computed << "\n";
	}

	INSTANTIATE_TEST_SUITE_P(simple_tests,
				 NibbleTestFixtureSmall,
				 ::testing::Values(std::make_tuple(0xf, 0xf, 1),
						   std::make_tuple(0xf, 0x7, 1),
						   std::make_tuple(0x11111, 0x1, 3),
						   std::make_tuple(0x11111, 0x4, 3)
						   )

				);

	class NibbleTestFixtureSmallDouble: public ::testing::TestWithParam<std::tuple<uint16_t, uint16_t, uint64_t, uint64_t>> {
        };


	TEST_P(NibbleTestFixtureSmallDouble, simple_tests) {
		
		uint16_t target  = std::get<0>(GetParam());
		uint16_t target2 = std::get<1>(GetParam());
		uint64_t query   = std::get<2>(GetParam());
		uint64_t answer  = std::get<3>(GetParam());
		
		std::vector<uint16_t> v{ target, target2 };
		auto computed = nibble_search(query, v);
		ASSERT_EQ(computed, answer) << "nibble_search(" << (int)query << ", [" << target << ", " << target2 << "]) == " << answer << " but your code produced " << computed << "\n";
	}

	INSTANTIATE_TEST_SUITE_P(simple_tests,
				 NibbleTestFixtureSmallDouble,
				 ::testing::Values(std::make_tuple(0xf,0xf, 0xf, 2),
						   std::make_tuple(0x1,0x1111, 0x1, 4)
						   )

				);

	class NibbleTestFixtureBig: public ::testing::TestWithParam<std::tuple<uint64_t, uint64_t, uint64_t>> {
        };

        TEST_P(NibbleTestFixtureBig, NibbleTest) {
		uint64_t target_count = std::get<0>(GetParam());
		uint64_t seed     = std::get<1>(GetParam());
		uint64_t answer     = std::get<2>(GetParam());
		uint64_t sum = run_nibble(target_count, seed);
		
		ASSERT_EQ(sum, answer);
	}

	INSTANTIATE_TEST_SUITE_P(VisibleTests,
				 NibbleTestFixtureBig,
				 ::testing::Values(std::make_tuple(10000,1, 7751),
						   std::make_tuple(100000, 2, 74646))

				);

#include"hidden_tests.cpp"
	
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
