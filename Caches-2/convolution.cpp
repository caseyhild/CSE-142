#include"cfiddle.hpp"
#include"tensor_t.hpp"
#include"util.hpp"
#include<cstdint>
#include<cassert>

//START
extern "C"
void do_convolution(const tensor_t<uint32_t> & source,
		    const tensor_t<uint32_t> & kernel,
		    tensor_t<uint32_t> & target) {
	disable_prefetcher();
	flush_caches();
	start_measurement();
	for(register int32_t i = 0; i < target.size.x; i++) {
		for(register int32_t j = 0; j < kernel.size.x; j++) {
			target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
		}
	}
	end_measurement();
}

extern "C"
void convolution(uint64_t source_size, uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	// TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	// TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	// TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	
	// Here's the the key part:
	do_convolution(source, kernel, target);
  
	// TAG_STOP("source");
	// TAG_STOP("kernel");
	// TAG_STOP("target");

}

//END


extern "C"
void do_convolution_new_loop(const tensor_t<uint32_t> & source,
			     const tensor_t<uint32_t> & kernel,
			     tensor_t<uint32_t> & target, int32_t tile_size) {

	start_measurement();
	for(int32_t i = 0; i < target.size.x; i++) {
		// We create a new loop variable jj that advanced one chunk at a time.
		for(int32_t jj = 0; jj < kernel.size.x; jj += tile_size) {
			// We iterate over the chunk.  The more complicated termination 
			// condition keeps ups from running off the end of the arry
			for(int32_t j = jj; j < kernel.size.x && j < jj + tile_size; j++) { 
				target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
			}
		}
	}
	end_measurement();
  

}

extern "C"
void convolution_new_loop(uint64_t source_size,
			       uint64_t kernel_size, 
			       uint64_t _target_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_new_loop(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");
}



extern "C"
void  do_convolution_split(const tensor_t<uint32_t> & source,
			  const tensor_t<uint32_t> & kernel,
			  tensor_t<uint32_t> & target, int32_t tile_size) {

	start_measurement();
	for(int32_t i = 0; i < target.size.x; i++) {
		for(int32_t jj = 0; jj < kernel.size.x; jj += 2048) { // We create a new loop variable jj that advanced one chunk at a time.
			for(int32_t j = jj; j < kernel.size.x && j < jj + 2048; j++) { // We iterate over the chunk.  The more complicated termination 
				// condition keeps us from running off the end of the array
				target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
			}
		}
	}
	end_measurement();
  

}

extern "C"
void convolution_split(uint64_t source_size, uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_split(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");

}




extern "C"
void do_convolution_tiled(const tensor_t<uint32_t> & source,
			  const tensor_t<uint32_t> & kernel,
			  tensor_t<uint32_t> & target, int32_t tile_size) {
	disable_prefetcher();
	flush_caches();
	
	start_measurement();
	for(int32_t jj = 0; jj < kernel.size.x; jj += tile_size) {  // Move the jj chunk loop outside
		for(int32_t i = 0; i < target.size.x; i++) {
			for(int32_t j = jj; j < kernel.size.x && j < jj + tile_size; j++) {
				target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
			}
		}
	}
	end_measurement();
}

extern "C"
void convolution_tiled(uint64_t source_size,  uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_tiled(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");
}


extern "C"
void  __attribute__((optimize("unroll-loops"))) do_convolution_tiled_unrolled(const tensor_t<uint32_t> & source,
									      const tensor_t<uint32_t> & kernel,
									      tensor_t<uint32_t> & target, int32_t tile_size) {

	start_measurement();
	for(int32_t jj = 0; jj < kernel.size.x; jj += tile_size) {  // Move the jj chunk loop outside
		for(int32_t i = 0; i < target.size.x; i++) {
			for(int32_t j = jj; j < kernel.size.x && j < jj + tile_size; j++) {
				target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
			}
		}
	}
	end_measurement();

}



extern "C"
void convolution_tiled_unrolled(uint64_t source_size,uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_tiled_unrolled(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");
}



extern "C"
void __attribute__((optimize("unroll-loops"))) do_convolution_tiled_split(const tensor_t<uint32_t> & source, 
									  const tensor_t<uint32_t> & kernel,
									  tensor_t<uint32_t> & target, int32_t tile_size) {

	int32_t real_tile_size = tile_size/8 * 8; // this clears the low 3 bits.  Check the assembly!
	assert(tile_size>=8);

	start_measurement();
	for(int32_t jj = 0; jj < kernel.size.x; jj += real_tile_size) {  // Move the jj chunk loop outside
		for(int32_t i = 0; i < target.size.x; i++) {
			if (jj + real_tile_size > kernel.size.x) {
				for(int32_t j = jj; j < kernel.size.x; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				} 
			} else {
				for(int32_t j = jj; j < jj + real_tile_size; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				}
			}
		}
	}
	end_measurement();
	

}

extern "C"
void convolution_tiled_split(uint64_t source_size, uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_tiled_split(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");
}

#if(0) // CURSED
extern "C"
void __attribute__((optimize("unroll-loops"))) do_convolution_unrolled_split(const tensor_t<uint32_t> & source, 
									     const tensor_t<uint32_t> & kernel,
									     tensor_t<uint32_t> & target, int32_t tile_size) {

	const int32_t real_tile_size = 8;

	start_measurement();
	for(int32_t i = 0; i < target.size.x; i++) {
		for(int32_t jj = 0; jj < kernel.size.x; jj += real_tile_size) {  // Move the jj chunk loop outside
			if (jj + real_tile_size > kernel.size.x) {
				for(int32_t j = jj; j < kernel.size.x; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				} 
			} else {
				for(int32_t j = jj; j < jj + real_tile_size; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				}
			}
		}
	}
	end_measurement();
	

}

extern "C"
void convolution_unrolled_split(uint64_t source_size, uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);
	//TAG_START("source", source.data, &source.as_vector(source.element_count()), true);
	//TAG_START("kernel", kernel.data, &kernel.as_vector(kernel.element_count()), true);
	//TAG_START("target", target.data, &target.as_vector(target.element_count()), true);

	// Here's the the key part:
	do_convolution_unrolled_split(source, kernel, target, tile_size);
    
	//TAG_STOP("source");
	//TAG_STOP("kernel");
	//TAG_STOP("target");
}
#endif

extern "C"
void  __attribute__((optimize("unroll-loops"))) do_convolution_tiled_fixed_tile(const tensor_t<uint32_t> & source, 
									  const tensor_t<uint32_t> & kernel,
									  tensor_t<uint32_t> & target, int32_t tile_size) {

#define real_tile_size 512

	
	start_measurement();
	for(int32_t jj = 0; jj < kernel.size.x; jj += real_tile_size) {  // Move the jj chunk loop outside
		for(int32_t i = 0; i < target.size.x; i++) {
			if (jj + real_tile_size > kernel.size.x) {
				for(int32_t j = jj; j < kernel.size.x; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				} 
			} else {
				for(int32_t j = jj; j < jj + real_tile_size; j++) {
					target.get(i,0,0,0) += source.get(i + j,0,0,0) * kernel.get(j,0,0,0);
				}
			}
		}
	}
	end_measurement();
	

}

extern "C"
void convolution_tiled_fixed_tile(uint64_t source_size, uint64_t kernel_size, int32_t tile_size) {
	tensor_t<uint32_t> source(source_size,1,1,1);
	tensor_t<uint32_t> kernel(kernel_size,1,1,1);
	uint64_t target_size = source_size - kernel_size;
	tensor_t<uint32_t> target(target_size,1,1,1);

	do_convolution_tiled_fixed_tile(source, kernel, target, tile_size);
}


//-O3 -funroll-all-loops
