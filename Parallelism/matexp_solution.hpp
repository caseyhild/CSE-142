#ifndef MATEXP_SOLUTION_INCLUDED
#define MATEXP_SOLUTION_INCLUDED
#include <cstdlib>

#include <unistd.h>
#include<cstdint>
#include<iostream>
#include"cfiddle.hpp"
#include"walltime.h"
#include"tensor_t.hpp"

#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
template<typename T>
void
__attribute__((noinline,optimize("O3")))
mult_solution(tensor_t<T> &C, const tensor_t<T> &A, const tensor_t<T> &B)
{
    // This is just textbook matrix multiplication.
    #pragma omp parallel for
    for(int i = 0; i < C.size.x; i++) {
        for(int j = 0; j < C.size.y; j++) {
            T sum = 0;
            for(int k = 0; k < B.size.x; k++) {
                sum += A.get(i,k) * B.get(k,j);
            }
            C.get(i,j) = sum;
        }
    }
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
// A simple function to copy the contents of one tensor into another.
template<typename T>
void
__attribute__((noinline,optimize("O3")))
copy_matrix_solution(tensor_t<T> & dst, const tensor_t<T> & src) {
    dst = src;
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
template<typename T>
void
__attribute__((noinline,optimize("O3")))
identity(tensor_t<T> & t) {
    #pragma omp parallel for
    for(int32_t x = 0; x < t.size.x; x++) {
        t.get(x,x) = 1;
    }
}

#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
template<typename T>
void __attribute__((noinline,optimize("O3"))) matexp_solution(tensor_t<T> & dst, const tensor_t<T> & A, uint32_t power,
                      int64_t p1=0,
                      int64_t p2=0,
                      int64_t p3=0,
                      int64_t p4=0,
                      int64_t p5=0) {
    
    if(power == 2) {
        mult_solution(dst, A, A);
        return;
    }
    // We binary exponentiation to compute A^power. 
    // First, we compute partial powers
    // products[i] will hold A^(2^i)
    std::vector<tensor_t<T>*> products;
        
    products.push_back(new tensor_t<T>(A));
    
    identity(*products[0]); // A^0

    products.push_back(new tensor_t<T>(A)); // A^1

    // Compute A^(2^i).  We only go up to 10, because the lab says p <= 1024
    products.push_back(new tensor_t<T>(A));
    mult_solution(*products[2], *products[1],  *products[1]);
    if (power >= 4) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[3], *products[2],  *products[2]);
    }
    if (power >= 8) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[4], *products[3],  *products[3]);
    }
    if (power >= 16) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[5], *products[4],  *products[4]);
    }
    if (power >= 32) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[6], *products[5],  *products[5]);
    }
    if (power >= 64) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[7], *products[6],  *products[6]);
    }
    if (power >= 128) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[8], *products[7],  *products[7]);
    }
    if (power >= 256) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[9], *products[8],  *products[8]);
    }
    if (power >= 512) {
        products.push_back(new tensor_t<T>(A));
        mult_solution(*products[10], *products[9],  *products[9]);
    }

    identity(dst);
    
    // If bit i of power is set, we should include the A^(2^(i+1)) in the final product.
    if (1 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[1]);
    }
    if (2 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[2]);
    }
    if (4 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[3]);
    }
    if (8 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[4]);
    }
    if (16 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[5]);
    }
    if (32 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[6]);
    }
    if (64 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[7]);
    }
    if (128 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[8]);
    }
    if (256 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[9]);
    }
    if (512 & power) {
        tensor_t<T> tmp(dst);
        mult_solution(dst, tmp, *products[10]);
    }
}
#pragma GCC pop_options
#endif



// Cfiddle-signature=d5726aa8ce6feec979edb262d7dbe29a
