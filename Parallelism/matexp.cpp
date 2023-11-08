#include"matexp_reference.hpp"
#if defined(TESTING_SOLUTION)
#  include"matexp_testing.hpp"
#elif defined(FAST_SOLUTION)
#  include"matexp_solutions/matexp_fast.hpp"
#else
#  include"matexp_solution.hpp"
#endif

#include"cfiddle.hpp"
#include<omp.h>
#include<sstream>

#define ELEMENT_TYPE uint64_t

typedef std::tuple<int, int> Bench;

std::vector<Bench> benches = {
	std::make_tuple(600, 2),
	std::make_tuple(350, 25),
	std::make_tuple(120, 320),
};

/*std::vector<Bench> benches = {
	std::make_tuple(60, 2),
	std::make_tuple(30, 25),
	std::make_tuple(10, 320),
	};*/

#define ITERATIONS 8

extern "C"
void bench_solution(uint32_t threads, uint64_t seed, uint32_t iterations, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
	omp_set_num_threads(threads);
		
	for(auto b : benches) {
		auto size = std::get<0>(b);
		auto power = std::get<1>(b);
		
		tensor_t<ELEMENT_TYPE> D(size,size);
		tensor_t<ELEMENT_TYPE> A(size,size);
		randomize(A, seed, 0, 1024);

		std::stringstream s;
		s << "size=" << size << " power=" << power;
		start_measurement(s.str().c_str());
		for(uint32_t i = 0; i < iterations; i++) {
			matexp_solution<ELEMENT_TYPE>(D, A, power, p1,p2,p3,p4,p5);
		}
		end_measurement();
	} 
}


extern "C"
void bench_reference(uint32_t threads, uint64_t seed, uint32_t iterations, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
	omp_set_num_threads(threads);
	
	for(auto b : benches) {
		auto size = std::get<0>(b);
		auto power = std::get<1>(b);
		
		tensor_t<ELEMENT_TYPE> D(size,size);
		tensor_t<ELEMENT_TYPE> A(size,size);
		randomize(A, seed, 0,1024);

		std::stringstream s;
		s << "size=" << size << " power=" << power;
		
		start_measurement(s.str().c_str());

		for(uint32_t i = 0; i < iterations; i++) {
			matexp_reference<ELEMENT_TYPE>(D, A, power);
		}
		end_measurement();
		
	} 
}



extern "C"
void matexp_reference_c(uint64_t size, uint32_t power, uint32_t threads, uint32_t iterations, uint64_t seed, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
	tensor_t<ELEMENT_TYPE> dst(size,size);
	tensor_t<ELEMENT_TYPE> A(size,size);
	randomize(A, seed, 0,1024);
	omp_set_num_threads(threads);
	start_measurement();
	for(uint32_t i = 0; i < iterations; i++) {
		matexp_reference<ELEMENT_TYPE>(dst, A, power, p1, p2, p3, p4, p5);
	}
	end_measurement();
}



extern "C"
void matexp_solution_c(uint64_t size, uint32_t power, uint32_t threads, uint32_t iterations, uint64_t seed, int64_t p1, int64_t p2, int64_t p3, int64_t p4, int64_t p5)
{
	tensor_t<ELEMENT_TYPE> dst(size,size);
	tensor_t<ELEMENT_TYPE> A(size,size);
	randomize(A, seed, 0,1024);
	omp_set_num_threads(threads);
	start_measurement();
	for(uint32_t i = 0; i < iterations; i++) {
		matexp_solution<ELEMENT_TYPE>(dst, A, power, p1, p2, p3, p4, p5);
	}
	end_measurement();
}

