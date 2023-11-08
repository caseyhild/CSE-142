
#include"cfiddle.hpp"
#include"matmul.hpp"


// Compute the matrix product: c = a * b
extern "C" void matrix_product(Matrix * c, Matrix * a, Matrix * b) {
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

// These are the functions you'll be modifying.   Right now, they all just call the reference
// implementation above, but when you are done, they should be specialized depending on how they
// are called by the go() function in the next cell.
extern "C" void matrix_product_1(Matrix * c, Matrix * a, Matrix * b) {
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

extern "C" void matrix_product_2(Matrix * c, Matrix * a, Matrix * b) {
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

extern "C" void matrix_product_3(Matrix * c, Matrix * a, Matrix * b) {
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

extern "C" void matrix_product_4(Matrix * c, Matrix * a, Matrix * b) {
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

extern "C" void matrix_product_5(Matrix * c, Matrix * a, Matrix * b) {
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

// Cfiddle-signature=429b0f70b07668cc2f3e8699ba3779c1