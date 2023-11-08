
CFIDDLE_INCLUDE=/cse142L/cfiddle/src/cfiddle/resources/include
CXX_STANDARD=-std=gnu++11
TARGET=x86_64-linux-gnu
MORE_LIBS=-pthread

include  /cse142L/cfiddle/src/cfiddle/resources/make/cfiddle.make
include /cse142L/cse141pp-archlab/cse141.make

$(BUILD)/run_tests.o: hidden_tests.cpp
$(BUILD)/run_tests.o: CXX_STANDARD=-std=gnu++17

run_tests.exe: $(BUILD)/run_tests.o $(BUILD)/ChunkAlloc.o
	$(CXX) $^ $(LDFLAGS) -L$(GOOGLE_TEST_ROOT)/lib -lgtest -lgtest_main  -o $@

regressions.json: run_tests.exe
	./run_tests.exe --gtest_output=json:$@

autograde.csv: AlignedAllocator.hpp Allocator.cpp canary.cpp
#	./lab_lint.py AlignedAllocator.hpp
	./run_bench.py --results $@ --source AlignedAllocator.hpp --optimize "-O3"
	pretty-csv autograde.csv
	rm -rf build run_tests.exe

.PHONY: autograde
autograde: regressions.json autograde.csv
