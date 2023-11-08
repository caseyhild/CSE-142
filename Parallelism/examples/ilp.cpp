#include<cstdint>
#include"cfiddle.hpp"

extern "C"
uint64_t wide_1(uint64_t size) {
	register uint64_t i =0;
	start_measurement();
	for(i = 0; i < size; i++) {
		i = i+1;
		i = i+1;       
		i = i+1;       
		i = i+1;       
		i = i+1;
	}
	end_measurement();
	return i;
}


extern "C"
uint64_t wide_2(uint64_t size) {
	register uint64_t a = 4;
	register uint64_t b = 4;
	register uint64_t c = 4;
	register uint64_t d= 4;
	register uint64_t e =4 ;
	register uint64_t f= size;

	start_measurement();
	for(register uint64_t i = 0; i < size; i++) {
		i = i+1; a = a+1; 
		i = i+1; a = a+1; 
		i = i+1; a = a+1; 
		i = i+1; a = a+1; 
		i = i+1; a = a+1; 
	}
	end_measurement();
	return a + b + c +d + e + f;
}


extern "C"
uint64_t wide_3(uint64_t size) {
	register uint64_t a = 4;
	register uint64_t b = 4;
	register uint64_t c = 4;
	register uint64_t d = 4;
	register uint64_t e =4 ;
	register uint64_t f= size;

	start_measurement();
	for(register uint64_t i = 0; i < size; i++) {
		i = i+1; a = a+1; b = b+1;  
		i = i+1; a = a+1; b = b+1;  
		i = i+1; a = a+1; b = b+1;  
		i = i+1; a = a+1; b = b+1;  
		i = i+1; a = a+1; b = b+1;  
	}
	end_measurement();
	return a + b + c +d + e + f;
}


extern "C"
uint64_t wide_4(uint64_t size) {
	register uint64_t a = 4;
	register uint64_t b = 4;
	register uint64_t c = 4;
	register uint64_t d = 4;
	register uint64_t e = 4 ;
	register uint64_t f= size;

	start_measurement();
	for(register uint64_t i = 0; i < size; i++) {
		i = i+1;  a = a+1; b = b+1;  d = d+1; 
		i = i+1;  a = a+1; b = b+1;  d = d+1; 
		i = i+1;  a = a+1; b = b+1;  d = d+1; 
		i = i+1;  a = a+1; b = b+1;  d = d+1; 
		i = i+1;  a = a+1; b = b+1;  d = d+1; 
	}
	end_measurement();
	return a + b + c +d + e + f;
}

extern "C"
uint64_t wide_5(uint64_t size) {
	register uint64_t a = 4;
	register uint64_t b = 4;
	register uint64_t c = 4;
	register uint64_t d= 4;
	register uint64_t e =4 ;
	register uint64_t f= size;

	start_measurement();
	for(register uint64_t i = 0; i < size; i++) {
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
	}
	end_measurement();
	return a + b + c +d + e + f;
}


