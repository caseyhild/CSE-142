#ifndef MATMUL_INCLUDED
#define MATMUL_INCLUDED

struct Matrix {
    uint columns;
    uint rows;
    uint64_t * data;
};

Matrix * matrix_new(uint columns, uint rows);
void matrix_delete(Matrix *m);
void matrix_zero(Matrix *m);
void matrix_random(Matrix *m, uint64_t * seed);
bool matrix_equal(Matrix *a, Matrix *b);
void matrix_copy(Matrix *a, Matrix *b);


extern "C" void matrix_product_1(Matrix * c, Matrix * a, Matrix * b);
extern "C" void matrix_product_2(Matrix * c, Matrix * a, Matrix * b);
extern "C" void matrix_product_3(Matrix * c, Matrix * a, Matrix * b);
extern "C" void matrix_product_4(Matrix * c, Matrix * a, Matrix * b);
extern "C" void matrix_product_5(Matrix * c, Matrix * a, Matrix * b);


// Read a value from (c,r) in m
inline uint64_t matrix_read(Matrix * m, uint c, uint r) {
    return m->data[r * m->columns + c];
}

// Write v to (x,y) in m
inline void matrix_write(Matrix * m, uint c, uint r, uint64_t v) {
    m->data[r * m->columns + c] = v;
}
#endif
