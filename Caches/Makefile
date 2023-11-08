SHELL=/bin/bash
.SUFFIXES:
default:

.PHONY: create-labs
create-labs:
	cse142 lab delete -f caches-bench
	cse142 lab delete -f caches
	cse142 lab create --name "Lab 3: Caches (Benchmark)" --short-name "caches-bench" --docker-image stevenjswanson/cse142l-runner:22fa-lab-3 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-11-8T23:59:59 --starter-repo https://github.com/CSE142/fa22-CSE142L-caches-starter.git --starter-branch main
	cse142 lab create --name "Lab 3: Caches" --short-name "caches" --docker-image stevenjswanson/cse142l-runner:22fa-lab-3 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-11-8T23:59:59

STUDENT_EDITABLE_FILES=SolutionAllocator.hpp
PRIVATE_FILES=Lab.key.ipynb admin .git solution

OPTIMIZE+=-march=x86-64
COMPILER=gcc-8
include $(ARCHLAB_ROOT)/cse141.make

.PHONY: autograde
autograde: alloc_main.exe regressions.json
	./alloc_main.exe --size $$[1024*128] --stat-set ./L1.cfg --MHz 3500  --stats bench.csv --function allocator_bench_solution allocator_bench_starter
	./alloc_main.exe --size 320000       --stat-set ./L1.cfg --stat-set microbench.cfg --MHz 3500 --stats microbench.csv --function allocator_microbench_starter allocator_microbench_solution
	./alloc_main.exe --size 4096         --stat-set ./L1.cfg --stats miss_machine.csv  --MHz 3500 --function miss_machine_solution miss_machine_starter 

run_tests.exe: $(BUILD)ChunkAlloc.o

regressions.json: run_tests.exe
	./run_tests.exe --gtest_output=json:$@

Allocator.exe:  $(BUILD)Allocator.o $(BUILD)ChunkAlloc.o

alloc_main.exe:  $(BUILD)alloc_main.o $(BUILD)ChunkAlloc.o $(BUILD)Allocator.o
alloc_main.exe: EXTRA_LDFLAGS=
$(BUILD)alloc_main.o : OPTIMIZE=$(ALLOC_OPTIMIZE)
$(BUILD)Allocator.o: OPTIMIZE=$(ALLOC_OPTIMIZE)
$(BUILD)Allocator.s: OPTIMIZE=$(ALLOC_OPTIMIZE)

fiddle.exe:  $(BUILD)fiddle.o $(FIDDLE_OBJS)
fiddle.exe: EXTRA_LDFLAGS=-pg
$(BUILD)fiddle.o : OPTIMIZE=-O3 -pg

# temporary fix.
run_tests.o: $(BUILD)run_tests.o
	cp $^ $@

