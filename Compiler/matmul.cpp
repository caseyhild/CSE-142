#include"cfiddle.hpp"
#include"matmul.hpp"

//START_IMPL
// Create a new matrix of a given size
Matrix * matrix_new(uint rows, uint columns) {
    Matrix *n = new Matrix;
    n->columns = columns;
    n->rows = rows;
    n->data = new uint64_t[columns * rows];
    return n;
}

// Delete a matrix
void matrix_delete(Matrix *m) {
    delete [] m->data;
}

// Set a matrix to zero
void matrix_zero(Matrix *m) {
    for (uint i = 0; i < m->rows *m->columns; i++){
        m->data[i] = 0;
    }
}

// Fill a matrix with random numbers
void matrix_random(Matrix *m, uint64_t * seed) {
    for (uint i = 0; i < m->rows *m->columns; i++){
        m->data[i] = fast_rand(seed);
    }
}
//END_IMPL

// Copy one matrix to another.
void matrix_copy(Matrix *a, Matrix *b) {
    for (uint i = 0; i < a->rows *a->columns; i++){
        a->data[i] = b->data[i];
    }
}

// Check for matrix equality.
bool matrix_equal(Matrix *a, Matrix *b) {
    if (a->rows != b->rows || a->columns != b->columns)
        return false;

    for (uint i = 0; i < a->rows *a->columns; i++){
        if (a->data[i] != b->data[i]) return false;
    }
    return true;
}

//START_TESTS
extern "C" void test_matrix_product_1(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	/////////////////////////////////////    

	Matrix *a1 = matrix_new(columns, size1);
	Matrix *b1 = matrix_new(size1, rows);
	Matrix *c1 = matrix_new(columns, rows);
	matrix_random(a1, &seed);
	matrix_random(b1, &seed);

	start_measurement();
	matrix_product_1(c1, a1, b1);
	end_measurement();

	matrix_delete(a1);
	matrix_delete(b1);
	matrix_delete(c1);
}


extern "C" void test_matrix_product_2(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	/////////////////////////////////////    
	
	Matrix *a2 = matrix_new(size2, size2);
	Matrix *b2 = matrix_new(size2, size2);
	Matrix *c2 = matrix_new(size2, size2);
	matrix_random(a2, &seed);
	matrix_random(b2, &seed);
    
	start_measurement();
    
	matrix_product_2(c2, a2, b2);
	end_measurement();

	matrix_delete(a2);
	matrix_delete(b2);
	matrix_delete(c2);
}
 
extern "C" void test_matrix_product_3(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	/////////////////////////////////////    

	Matrix *a3 = matrix_new(1 << log_size, 1 << log_size);
	Matrix *b3 = matrix_new(1 << log_size, 1 << log_size);
	Matrix *c3 = matrix_new(1 << log_size, 1 << log_size);
	matrix_random(a3, &seed);
	matrix_random(b3, &seed);
    
	start_measurement();
    
	matrix_product_3(c3, a3, b3);

	end_measurement();

	matrix_delete(a3);
	matrix_delete(b3);
	matrix_delete(c3);
}
	      
extern "C" void test_matrix_product_4(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	/////////////////////////////////////    
	
#define SIZE 512
	Matrix *a4 = matrix_new(SIZE, SIZE);
	Matrix *b4 = matrix_new(SIZE, SIZE);
	Matrix *c4 = matrix_new(SIZE, SIZE);
	matrix_random(a4, &seed);
	matrix_random(b4, &seed);

	start_measurement();

	matrix_product_4(c4, a4, b4);

	end_measurement();
	matrix_delete(a4);
	matrix_delete(b4);
	matrix_delete(c4);
}
 
extern "C" void test_matrix_product_5(uint rows, uint size1, uint columns, uint size2, uint log_size, uint64_t seed) {
	/////////////////////////////////////    

	Matrix *a5 = matrix_new(SIZE, SIZE);
	Matrix *b5 = matrix_new(SIZE, SIZE);
	Matrix *c5 = matrix_new(SIZE, SIZE);
	seed = 1;
	matrix_random(a5, &seed);
	matrix_random(b5, &seed);

	start_measurement();

	matrix_product_5(c5, a5, b5);

	end_measurement();

	/////////////////////////////////////    
	matrix_delete(a5);
	matrix_delete(b5);
	matrix_delete(c5);

}
//END_TESTS
