
extern "C"
uint64_t* high_ilp(uint64_t threads, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	register uint64_t a = 4;
	register uint64_t b = 4;
	register uint64_t c = 4;
	register uint64_t d= 4;
	register uint64_t e =4 ;
	register uint64_t f= size;

	for(register uint64_t i = 0; i < size; i++) {
		i = i+1;  a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;	 a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;	 a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;	 a = a+1; b = b+1;  d = d+1; e = e+1;
		i = i+1;	 a = a+1; b = b+1;  d = d+1; e = e+1;
	}
	data[0] = a + b + c +d + e + f;
	return data;
}


extern "C"
uint64_t* low_ilp(uint64_t threads, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	register uint64_t i =0;
	for(i = 0; i < size; i++) {
		i = i+1;
		i = i+1;       
		i = i+1;       
		i = i+1;       
		i = i+1;
	}
	data[0] = i;
	return data;
}



//START
extern "C"
uint64_t* high_mlp(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	MissMachine a(arg1, size);

#define MACHINES  8
	a.make_links();
	register MissingLink *al = a.links[0* (a.link_count/MACHINES)];
	register MissingLink *bl = a.links[1* (a.link_count/MACHINES)];
	register MissingLink *cl = a.links[2* (a.link_count/MACHINES)];
	register MissingLink *dl = a.links[3* (a.link_count/MACHINES)];
	register MissingLink *el = a.links[4* (a.link_count/MACHINES)];
	register MissingLink *fl = a.links[5* (a.link_count/MACHINES)];
	register MissingLink *gl = a.links[6* (a.link_count/MACHINES)];
	register MissingLink *hl = a.links[7* (a.link_count/MACHINES)];

#define ITERATIONS arg3
	for(unsigned int i = 0; i < ITERATIONS; i++) {
		al = al->next;
		bl = bl->next;
		cl = cl->next;
		dl = dl->next;
		el = el->next;
		fl = fl->next;
		gl = gl->next;
		hl = hl->next;
	}

	return reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(al) +
					   reinterpret_cast<uintptr_t>(bl) +
					   reinterpret_cast<uintptr_t>(cl) +
					   reinterpret_cast<uintptr_t>(dl) +
					   reinterpret_cast<uintptr_t>(el) + 
					   reinterpret_cast<uintptr_t>(fl) + 
					   reinterpret_cast<uintptr_t>(gl) + 
					   reinterpret_cast<uintptr_t>(hl) + 
		);
} 

//START
extern "C"
uint64_t* low_mlp(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	MissMachine a(arg1, size);
	
#define MACHINES  1
	a.make_links();
	register MissingLink *al = a.links[0* (a.link_count/MACHINES)];
	
#define ITERATIONS arg3
	for(unsigned int i = 0; i < ITERATIONS; i++) {
		al = al->next;
	}

	return reinterpret_cast<uint64_t*>(reinterpret_cast<uintptr_t>(al));
} 


extern "C"
uint64_t* low_ilp_low_mlp(uint64_t thread_count, uint64_t * data, uint64_t size, uint64_t arg1, uint64_t arg2, uint64_t arg3)  {

	int hyperthreading = arg1 != 0;

	std::thread thread0;

	std::thread **threads = new std::thread*[thread_count];
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i] = new std::thread(private2_histogram,
					     i,
					     thread_count,
					     private_buckets,
					     &data[chunk_length * i],
					     chunk_length);
	}

	private2_histogram(thread_count - 1,
			  thread_count,
			  private_buckets,
			  &data[chunk_length * (thread_count - 1)],
			  size-(chunk_length * (thread_count - 1)));  // catch last bit due to rounding.
	
	for(unsigned int i = 0; i < thread_count - 1; i++) {
		threads[i]->join();
	}


	for(unsigned int id = 0; id < thread_count; id++) {
		for(int b = 0; b < 256; b++) {
			histogram[b] += private_buckets[id*256 + b]++;
		}
	}

	for(unsigned int i = 0; i < thread_count - 1; i++) {
		delete threads[i];
	}
	delete threads;
	delete private_buckets;
	return data;

}
	

