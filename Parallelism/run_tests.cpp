#define ENABLE_GTEST_FOR_TENSORS
#ifdef TESTING_SOLUTION
#include"matexp_testing.hpp"
#else
#include"matexp_solution.hpp"
#endif
#include"matexp_reference.hpp"
#include <iostream>
#include "gtest/gtest.h"
#include <sstream>
#include"archlab.hpp"
#include"params.hpp"
#include"omp.h"
#include"tensor_t.hpp"

#define ELEMENT_TYPE uint64_t

#define PARAM_LOOP(x)	for(auto & p##x: ps##x ) 
#define PARAM_MARK(x)	theDataCollector->register_tag("p"#x,p##x)
#define PARAM_PASS(x)   p##x
#define PARAM(x)	std::vector<int64_t> ps##x;			\
	std::vector<int64_t> default_ps##x;				\
	default_ps##x.push_back(1);					\
	archlab_add_multi_option<std::vector<int64_t> >("p" #x,		\
							ps##x,		\
							default_ps##x,	\
							"1",		\
							"Parameter " #x ".");

uint64_t master_seed;
int64_t gp1;
int64_t gp2;
int64_t gp3;
int64_t gp4;
int64_t gp5;
int64_t gsize;
int64_t gpower;

namespace Tests {

	class MatexpTests :  public ::testing::Test {
	};


//START1
	void do_simple_diag_test(int size, int power) {
		tensor_t<ELEMENT_TYPE> A(size,size);
		tensor_t<ELEMENT_TYPE> B(size,size);
		for(int j = 0; j < size; j++) {
			A(j,j) = 2;
			B(j,j) = 1 << power;
		}
		
		tensor_t<ELEMENT_TYPE> C(size,size);
		
		matexp_solution(C, A, power, gp1, gp2, gp3, gp4, gp5);
		ASSERT_TENSORS_EQ(ELEMENT_TYPE, C,B) << "diagonal matrix squaring check failed\n" << A << "\nRAISED TO THE " << power << " SHOULD BE  \n" << B << "\nYOUR CODE GOT\n" << C<< "\n";
	}

	void do_simple_offdiag_test(int size, int power) {
		tensor_t<ELEMENT_TYPE> A(size,size);
		tensor_t<ELEMENT_TYPE> B(size,size);
		for(int j = 0; j < size; j++) {
			A(j,size-j-1) = 2;
			if ((power % 2) == 0) {
				B(j,j) = 1 << power; // for even power the result is diagonal
			} else {
				B(j,size-j-1) = 1 << power; // for odd powers its off-diagonal.
								
			}
		}
		
		tensor_t<ELEMENT_TYPE> C(size,size);
		
		matexp_solution(C, A, power, gp1, gp2, gp3, gp4, gp5);
		ASSERT_TENSORS_EQ(ELEMENT_TYPE, C,B) << "off-diagonal matrix squaring check failed\n" << A << "\nRAISED TO THE " << power << " SHOULD BE  \n" << B << "\nYOUR CODE GOT\n" << C<< "\n";
	}

//END1
	TEST_F(MatexpTests, one_test) {
		std::cerr << "Running one_test with size=" << gsize << "; power=" << gpower <<"\n";
		do_simple_diag_test(gsize, gpower);
		do_simple_offdiag_test(gsize, gpower);
	}


	TEST_F(MatexpTests, simple_tests) {
		do_simple_diag_test(2,0);
		do_simple_diag_test(3,2);
		do_simple_diag_test(4,3);
		do_simple_diag_test(5,4);
		do_simple_diag_test(6,5);

		do_simple_offdiag_test(2,0);
		do_simple_offdiag_test(3,2);
		do_simple_offdiag_test(4,3);
		do_simple_offdiag_test(5,4);
		do_simple_offdiag_test(6,5);
	}

	
	TEST_F(MatexpTests, simple_random_tests) {
		uint64_t seed = master_seed;
		for(int i = 0; i < 20; i++){
			int size = (fast_rand(&seed) % 100)+1;
			int power = (fast_rand(&seed) % 10)+1;
			do_simple_diag_test(size, power);
			do_simple_offdiag_test(size, power);
		}
	}

//START2
	void do_test(int size, int power, uint64_t seed) {
		tensor_t<ELEMENT_TYPE> D1(size,size);
		tensor_t<ELEMENT_TYPE> D2(size,size);
		tensor_t<ELEMENT_TYPE> A(size,size);
		randomize(A, seed, 0,10);//0.5, 1.5);

		tensor_t<ELEMENT_TYPE> B(A);
		ASSERT_TENSORS_EQ(ELEMENT_TYPE, A,B) << "pre equality check for A and B failed for size " << (size) << "; power = " << power << "; seed = " << master_seed << "\n";
		ASSERT_TENSORS_EQ(ELEMENT_TYPE, D1,D2) << "pre equality check for D1 and D2 failed for size " << (size) << "; power = " << power << "; seed = " << master_seed << "\n";
		matexp_reference(D1, A, power);
		matexp_solution(D2, B, power, gp1, gp2, gp3, gp4, gp5);
		ASSERT_TENSORS_EQ(ELEMENT_TYPE, D1,D2) << "exponetiation check failed for size " << (size) << "; power = " << power << "; seed = " << master_seed << "\n";
	}
//END2	
	
	class MatexpBench :  public ::testing::Test {
	};


#define HIDDEN_SEEDS
#include"hidden_tests.cpp"
	
	TEST_F(MatexpBench, bench_tests) {
		std::vector<uint64_t> seeds = {master_seed HIDDEN_SEEDS};
		std::cerr << "Running with " << seeds.size() << " seeds.\n";
		for(auto seed :seeds) {
			do_test(600, 2, seed);
			do_test(350, 25, seed);
			do_test(120, 320, seed);
		}
	}	

	TEST_F(MatexpTests, randomize_tests) {
		uint64_t seed = master_seed;
		for (int i =0; i < 10; i++) {
			int size = (fast_rand(&seed) % 10)+1 ;
			int power = (fast_rand(&seed) % 20)+1 ;
			std::cerr << "size = " << size  << "; power = " << power << "\n";
			do_test(size, power, seed);
		}
	}

	class MatexpTestFixture: public ::testing::TestWithParam<std::tuple<int, int>> {
	};


	TEST_P(MatexpTestFixture, ExpTest) {
		int size = std::get<0>(GetParam());
		int power = std::get<1>(GetParam());
		do_test(size, power, master_seed);
	}

	INSTANTIATE_TEST_CASE_P(
		MatexpTests,
		MatexpTestFixture,
		::testing::Values(
			std::make_tuple(1, 0),
			std::make_tuple(8, 0),
			std::make_tuple(64, 0),
			std::make_tuple(3, 0),
			std::make_tuple(27, 0),
			std::make_tuple(243, 0),

			std::make_tuple(1, 1),
			std::make_tuple(8, 1),
			std::make_tuple(64, 1),
			std::make_tuple(3, 1),
			std::make_tuple(27, 1),
			std::make_tuple(243, 1),

			std::make_tuple(1, 2),
			std::make_tuple(8, 2),
			std::make_tuple(64, 2),
			std::make_tuple(4, 2),
			std::make_tuple(26, 2),
			std::make_tuple(242, 2),

			std::make_tuple(1, 3),
			std::make_tuple(8, 3),
			std::make_tuple(64, 3),
			std::make_tuple(5, 3),
			std::make_tuple(23, 3),
			std::make_tuple(240, 3),

			std::make_tuple(1, 5),
			std::make_tuple(8, 5),
			std::make_tuple(64, 5),
			std::make_tuple(4, 5),
			std::make_tuple(29, 5),
			std::make_tuple(239, 5),

			std::make_tuple(1, 7),
			std::make_tuple(8, 7),
			std::make_tuple(64, 7),
			std::make_tuple(3, 7),
			std::make_tuple(25, 7),
			std::make_tuple(245, 7),

			std::make_tuple(2, 50),
			std::make_tuple(8, 25),
			std::make_tuple(64, 20),
			std::make_tuple(3, 50),
			std::make_tuple(25, 25),
			std::make_tuple(245, 15)
			)

		);
		

}


int main(int argc, char **argv) {
	if (argc >= 2) {
		if (!strcmp(argv[1], "--print-deltas")) {
			tensor_t<ELEMENT_TYPE>::diff_prints_deltas = true;
			tensor_t<double>::diff_prints_deltas = true;
			tensor_t<float>::diff_prints_deltas = true;
			argc--;
			argv++;
		}
	}
	std::vector<uint64_t> seeds;
	std::vector<uint64_t> default_seeds;
	default_seeds.push_back(0xDEADBEEF);
	archlab_add_multi_option<std::vector<uint64_t> >("seed",
							 seeds,
							 default_seeds,
							 "0xDEADBEEF",		
							 "random seeds to run");
	std::vector<unsigned long int> powers;
	std::vector<unsigned long int> default_powers;
	default_powers.push_back(16);
	archlab_add_multi_option<std::vector<unsigned long int> >("power",
								  powers,
								  default_powers,
								  "16",
								  "Power.  Pass multiple values to run with multiple sizes.");
	std::vector<unsigned long int> threads;
	std::vector<unsigned long int> default_threads;
	default_threads.push_back(1);
	archlab_add_multi_option<std::vector<unsigned long int> >("thread",
					      threads,
					      default_threads,
					      "1",
					      "Thread.  Pass multiple values to run with multiple arg3s.");
		
	std::vector<unsigned long int> sizes;
	std::vector<unsigned long int> default_sizes;
	default_sizes.push_back(16);
	archlab_add_multi_option<std::vector<unsigned long int> >("size",
								  sizes,
								  default_sizes,
								  "16",
								  "Size.  Pass multiple values to run with multiple sizes.");
	
	PARAM(1);
	PARAM(2);
	PARAM(3);
	PARAM(4);
	PARAM(5);
	::testing::InitGoogleTest(&argc, argv);
	archlab_parse_cmd_line(&argc, argv);
	for(auto & s: seeds ) {
		master_seed = s;
		for(auto & thread: threads ) {
			omp_set_num_threads(thread);
		for(auto & size:sizes) {
			for(auto & power: powers ) {
				PARAM_LOOP(1) {
					PARAM_LOOP(2) {
						PARAM_LOOP(3) {
							PARAM_LOOP(4) {
								PARAM_LOOP(5){
									gp1 = p1;
									gp2 = p2;
									gp3 = p3;
									gp4 = p4;
									gp5 = p5;
									gsize = size;
									gpower = power;
									int r  = RUN_ALL_TESTS();
									std::cout << "Ran with...\n";
									std::cout << "seed = " <<  master_seed << "\n";
									std::cout << "p1 = " <<  p1 << "\n";
									std::cout << "p2 = " <<  p2 << "\n";
									std::cout << "p3 = " <<  p3 << "\n";
									std::cout << "p4 = " <<  p4 << "\n";
									std::cout << "p5 = " <<  p5 << "\n";
									std::cout << "thread count" << " = " << thread << "\n";
									if (r != 0) {
										std::cout << "Tests failed for this set of parameters.\n";
										return r;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	}
}


