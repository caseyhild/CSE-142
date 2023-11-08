
#ifndef MATEXP_REFERENCE_INCLUDED
#define MATEXP_REFERENCE_INCLUDED
#include <cstdlib>

#include <unistd.h>
#include<cstdint>
#include<iostream>
#include"cfiddle.hpp"
#include"walltime.h"
#include"tensor_t.hpp"

template<typename T>
void
__attribute__((noinline,optimize("Og")))
mult_reference(tensor_t<T> &C, const tensor_t<T> &A, const tensor_t<T> &B)
{
    // This is just textbook matrix multiplication.
    
    for(int i = 0; i < C.size.x; i++) {
        for(int j = 0; j < C.size.y; j++) {
            C.get(i,j) = 0;
            for(int k = 0; k < B.size.x; k++) {
                C.get(i,j) += A.get(i,k) * B.get(k,j);
            }
        }
    }
}


// A simple function to copy the contents of one tensor into another.
template<typename T>
void
__attribute__((noinline,optimize("Og")))
copy_matrix_reference(tensor_t<T> & dst, const tensor_t<T> & src) {
    for(int32_t x = 0; x < dst.size.x; x++)
        for(int32_t y = 0; y < dst.size.y; y++)
            dst.get(x,y) = src.get(x,y);
}

template<typename T>
void
__attribute__((noinline,optimize("Og")))
identity_reference(tensor_t<T> & t) {
    for(int32_t x = 0; x < t.size.x; x++) {
        for(int32_t y = 0; y < t.size.y; y++) {
            if (x == y) {
                t.get(x,y) = 1;
            } else {
                t.get(x,y) = 0;
            }
        }
    }
}

template<typename T>
void __attribute__((noinline)) matexp_reference(tensor_t<T> & dst, const tensor_t<T> & A, uint32_t power, 
                      int64_t p1=0,
                      int64_t p2=0,
                      int64_t p3=0,
                      int64_t p4=0,
                      int64_t p5=0) {

    double started = wall_time();

    // We binary exponentiation to compute A^power. 
    // First, we compute partial powers
    // products[i] will hold A^(2^i)
    std::vector<tensor_t<T>*> products;
        
    products.push_back(new tensor_t<T>(A));
    
    identity_reference(*products[0]); // A^0

    products.push_back(new tensor_t<T>(A)); // A^1

    // Compute A^(2^i).  We only go up to 10, because the lab says p <= 1024
    for(unsigned int i = 2; i <= 10; i++) {
        products.push_back(new tensor_t<T>(A));
        mult_reference(*products[i], *products[i-1],  *products[i-1]);
        if (power < (1u << i)) {
            break;
        }
    }

    identity_reference(dst);
    
    // If bit i of power is set, we should include the A^(2^(i+1)) in the final product.
    for(unsigned int i = 0; i < 10; i++) {

        if ((1 << i) & power) {
            tensor_t<T> tmp(dst);
            mult_reference(dst, tmp, *products[i+1]);
        }
        if (power <= (1u << i)) {
            break;
        }
    }

    for (auto p: products) {
	delete p;
    }
    
    double finished = wall_time();
    std::cerr << "That took " << finished - started << " seconds\n";

}
#endif

