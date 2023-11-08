#include "gtest/gtest.h"
#include<algorithm>
#include"fastrand.h"
#include <random>
#include<cassert>
#include"matmul.hpp"

namespace Tests {

	extern "C" void matrix_product_reference(Matrix * c, Matrix * a, Matrix * b) {
		for(uint i = 0; i < a->rows; i++) {    
			for(uint j = 0; j < b->columns; j++) {    
				matrix_write(c, j, i, 0);
				for(uint k = 0; k < a->columns; k++) {    
					matrix_write(c, j, i,
						     matrix_read(c, j, i) +
						     (matrix_read(a, k, i) * 
						      matrix_read(b, j, k)));
				}
			}
		}
	}

	//////////////////////////////////////////
	// These tests are for the correctness of the reference implementation.
	class ReferenceTest: public ::testing::Test {};
	
	TEST_F(ReferenceTest, square1) {
		Matrix *a = matrix_new(4, 4);
		Matrix *b = matrix_new(4, 4);
		Matrix *c = matrix_new(4, 4);
		{
			uint64_t a_data[] = {
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1};
				   
			uint64_t b_data[] = {
				1,2,3,4,
				4,1,2,3,
				3,4,1,2,
				2,3,4,1};

			a->data = a_data;
			b->data = b_data;
			
			matrix_product_reference(c, a, b);

			ASSERT_EQ(memcmp(c->data, b->data, sizeof(uint64_t)*c->rows * b->rows), 0);
		}
	}
	TEST_F(ReferenceTest, square2) {
		Matrix *a = matrix_new(4, 4);
		Matrix *b = matrix_new(4, 4);
		Matrix *c = matrix_new(4, 4);

		{
			uint64_t a_data[] = {
				2,0,0,0,
				0,2,0,0,
				0,0,2,0,
				0,0,0,2};
				   
			uint64_t b_data[] = {
				1,2,3,4,
				4,1,2,3,
				3,4,1,2,
				2,3,4,1};

			uint64_t c_data[] = {
				2,4,6,8,
				8,2,4,6,
				6,8,2,4,
				4,6,8,2};

			a->data = a_data;
			b->data = b_data;
			
			matrix_product_reference(c, a, b);

			ASSERT_EQ(memcmp(c->data, c_data, sizeof(uint64_t)*c->rows * b->rows), 0);
		}
	}
	
	TEST_F(ReferenceTest, square3) {
		Matrix *a = matrix_new(4, 4);
		Matrix *b = matrix_new(4, 4);
		Matrix *c = matrix_new(4, 4);
		
		{
			uint64_t a_data[] = {
				0,0,0,2,
				0,0,2,0,
				0,2,0,0,
				2,0,0,0};
				   
			uint64_t b_data[] = {
				1,2,3,4,
				4,1,2,3,
				3,4,1,2,
				2,3,4,1};

			uint64_t c_data[] = {
				4,6,8,2,
				6,8,2,4,
				8,2,4,6,
				2,4,6,8,
			};

				
			a->data = a_data;
			b->data = b_data;
			
			matrix_product_reference(c, a, b);
			
			ASSERT_EQ(memcmp(c->data, c_data, sizeof(uint64_t)*c->rows * b->rows), 0);
		}
		
	}

	TEST_F(ReferenceTest, not_square1) {
		Matrix *a = matrix_new(2, 4);
		Matrix *b = matrix_new(4, 2);
		Matrix *c = matrix_new(2, 2);
		{
			uint64_t a_data[] = {
				1, 2, 3, 4,
				5, 6, 7, 8};

			uint64_t b_data[] = {
				1,2,
				1,2,
				1,2,
				1,2};

			a->data = a_data;
			b->data = b_data;
			
			matrix_product_reference(c, a, b);

			uint64_t c_data[] = {
				10, 20,
				26, 52};
				

			ASSERT_EQ(memcmp(c_data, c->data, sizeof(uint64_t)*c->rows * c->columns), 0);
		}
	}

	TEST_F(ReferenceTest, not_square2) {
		Matrix *a = matrix_new(2, 4);
		Matrix *b = matrix_new(4, 3);
		Matrix *c = matrix_new(2, 3);
		{
			uint64_t a_data[] = {
				1, 2, 3, 4,
				5, 6, 7, 8};

			uint64_t b_data[] = {
				1,2,3,
				1,2,3,
				1,2,3,
				1,2,3};

			a->data = a_data;
			b->data = b_data;
			
			matrix_product_reference(c, a, b);

			uint64_t c_data[] = {
				10, 20, 30,
				26, 52, 78
			};
				

			ASSERT_EQ(memcmp(c_data, c->data, sizeof(uint64_t)*c->rows * c->columns), 0);
		}
	}

	////////////////////////////////////////////////////////////////
	// Here are the tests of your implementation.
	void test_matrix_product_1(uint rows, uint size1, uint columns, uint64_t seed) {
		Matrix *a = matrix_new(columns, size1);
		Matrix *b = matrix_new(size1, rows);
		matrix_random(a, &seed);
		matrix_random(b, &seed);

		Matrix *c = matrix_new(columns, rows);
		Matrix *c_ref = matrix_new(columns, rows);

		matrix_product_reference(c_ref, a,b);
		matrix_product_1(c, a,b);
		ASSERT_EQ(matrix_equal(c,c_ref), true);

		matrix_delete(a);
		matrix_delete(b);
		matrix_delete(c);
		matrix_delete(c_ref);
	}

	class MatmulTests_1: public ::testing::TestWithParam<std::tuple<uint16_t, uint16_t, uint16_t, uint64_t>> {
        };


	TEST_P(MatmulTests_1, matrix_product) {
		uint16_t columns  = std::get<0>(GetParam());
		uint16_t size2 = std::get<1>(GetParam());
		uint16_t rows = std::get<2>(GetParam());
		uint64_t seed    = std::get<3>(GetParam());
		test_matrix_product_1(columns, size2, rows, seed);
	}

	INSTANTIATE_TEST_SUITE_P(matrix_product,
				 MatmulTests_1,
				 ::testing::Values(std::make_tuple(1,1,1,1),
						   std::make_tuple(10,20,30,10),
						   std::make_tuple(32,20,10,10),
						   std::make_tuple(30,30,30,10)
						   ));

	////////////////////////////////////////////////////////////////
	void test_matrix_product_2(uint size2, uint64_t seed) {
		Matrix *a = matrix_new(size2, size2);
		Matrix *b = matrix_new(size2, size2);
		matrix_random(a, &seed);
		matrix_random(b, &seed);

		Matrix *c = matrix_new(size2, size2);
		Matrix *c_ref = matrix_new(size2, size2);

		matrix_product_reference(c_ref, a,b);
		matrix_product_2(c, a,b);
		ASSERT_EQ(matrix_equal(c,c_ref), true);

		matrix_delete(a);
		matrix_delete(b);
		matrix_delete(c);
		matrix_delete(c_ref);
	}

	class MatmulTests_2: public ::testing::TestWithParam<std::tuple<uint16_t, uint64_t>> {
        };


	TEST_P(MatmulTests_2, matrix_product_2) {
		uint16_t size1 = std::get<0>(GetParam());
		uint64_t seed    = std::get<1>(GetParam());
		test_matrix_product_2(size1, seed);
	}

	INSTANTIATE_TEST_SUITE_P(matrix_product_2,
				 MatmulTests_2,
				 ::testing::Values(std::make_tuple(1,1),
						   std::make_tuple(10,10),
						   std::make_tuple(32,10),
						   std::make_tuple(30,10)
						   ));

	////////////////////////////////////////////////////////////////

	void test_matrix_product_3(uint log_size, uint64_t seed) {
		Matrix *a = matrix_new(1 << log_size, 1 << log_size);
		Matrix *b = matrix_new(1 << log_size, 1 << log_size);
		matrix_random(a, &seed);
		matrix_random(b, &seed);

		Matrix *c = matrix_new(1 << log_size, 1 << log_size);
		Matrix *c_ref = matrix_new(1 << log_size, 1 << log_size);

		matrix_product_reference(c_ref, a,b);
		matrix_product_3(c,a,b);
		ASSERT_EQ(matrix_equal(c,c_ref), true);

		matrix_delete(a);
		matrix_delete(b);
		matrix_delete(c);
		matrix_delete(c_ref);
	}

	class MatmulTests_3: public ::testing::TestWithParam<std::tuple<uint16_t, uint64_t>> {
        };


	TEST_P(MatmulTests_3, matrix_product_3) {
		uint16_t log_size = std::get<0>(GetParam());
		uint64_t seed    = std::get<1>(GetParam());
		test_matrix_product_3(log_size, seed);
	}

	INSTANTIATE_TEST_SUITE_P(matrix_product_3,
				 MatmulTests_3,
				 ::testing::Values(std::make_tuple(1,1),
						   std::make_tuple(2,11),
						   std::make_tuple(4,12),
						   std::make_tuple(6,13)
						   ));
	////////////////////////////////////////////////////////////////
	void test_matrix_product_4(uint64_t seed) {
		Matrix *a = matrix_new(512, 512);
		Matrix *b = matrix_new(512, 512);
		matrix_random(a, &seed);
		matrix_random(b, &seed);

		Matrix *c = matrix_new(512, 512);
		Matrix *c_ref = matrix_new(512, 512);

		matrix_product_reference(c_ref, a,b);
		matrix_product_4(c, a,b);
		ASSERT_EQ(matrix_equal(c,c_ref), true);

		matrix_delete(a);
		matrix_delete(b);
		matrix_delete(c);
		matrix_delete(c_ref);
	}

	class MatmulTests_4: public ::testing::TestWithParam<std::tuple<uint64_t>> {
        };


	TEST_P(MatmulTests_4, matrix_product_4) {
		uint16_t seed = std::get<0>(GetParam());
		test_matrix_product_4(seed);
	}

	INSTANTIATE_TEST_SUITE_P(matrix_product_4,
				 MatmulTests_4,
				 ::testing::Values(std::make_tuple(1),
						   std::make_tuple(2)
						   ));
	////////////////////////////////////////////////////////////////
	void test_matrix_product_5() {
		Matrix *a = matrix_new(512, 512);
		Matrix *b = matrix_new(512, 512);
		uint64_t seed = 1;
		matrix_random(a, &seed);
		matrix_random(b, &seed);

		Matrix *c = matrix_new(512, 512);
		Matrix *c_ref = matrix_new(512, 512);

		matrix_product_reference(c_ref, a,b);
		matrix_product_5(c, a,b);
		ASSERT_EQ(matrix_equal(c,c_ref), true);

		matrix_delete(a);
		matrix_delete(b);
		matrix_delete(c);
		matrix_delete(c_ref);
	}

	class MatmulTests_5: public ::testing::Test {
        };


	TEST_F(MatmulTests_5, matrix_product_5) {
		test_matrix_product_5();
	}
#include"hidden_tests.cpp"

}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
