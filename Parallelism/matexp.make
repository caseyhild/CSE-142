CFIDDLE_INCLUDE=/cse142L/cfiddle/src/cfiddle/resources/include
CXX_STANDARD=-std=gnu++17
TARGET=x86_64-linux-gnu
MORE_LIBS=-pthread -lboost_program_options -lgomp

STUDENT_EDITABLE_FILES=matexp_solution.hpp 

include  /cse142L/cfiddle/src/cfiddle/resources/make/cfiddle.make
include /cse142L/cse141pp-archlab/cse141.make

$(BUILD)/run_tests.o: hidden_tests.cpp archlab.cpp matexp_solution.hpp  matexp_reference.hpp

run_tests.exe: $(BUILD)/run_tests.o $(BUILD)/archlab.o
	$(CXX) $^ $(LDFLAGS) -L$(GOOGLE_TEST_ROOT)/lib -lgtest -lgtest_main  -o $@

regressions.json: run_tests.exe
	./run_tests.exe --thread 12 --gtest_output=json:$@

autograde.csv: matexp.cpp matexp_solution.hpp canary.cpp
	./run_bench.py --results $@
	pretty-csv autograde.csv
	rm -rf build run_tests.exe

.PHONY: matexp_clean
matexp_clean:
	rm -rf run_tests.exe
clean: matexp_clean

.PHONY: autograde
autograde: regressions.json autograde.csv


$(BUILD)/run_tests.o: OPTIMIZE=-fopenmp
