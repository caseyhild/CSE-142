.SUFFIXES:
default:

ENCRYPTED_FILES=hidden_tests.cpp autograde_seed
PRIVATE_FILES=Lab.key.ipynb admin .git matmul_fast.cpp $(ENCRYPTED_FILES)
STUDENT_EDITABLE_FILES=matmul_solution.cpp


.PHONY: create-labs
create-labs:
	cse142 lab delete -f compiler-bench
	cse142 lab delete -f compiler
	cse142 lab create --name "Lab 2: The Compiler (Benchmark)" --short-name "compiler-bench" --docker-image stevenjswanson/cse142l-runner:22fa-lab-2 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-10-25T23:59:59 --starter-repo https://github.com/CSE142/fa22-CSE142L-compiler-starter.git --starter-branch main
	cse142 lab create --name "Lab 2: The Compiler" --short-name "compiler" --docker-image stevenjswanson/cse142l-runner:22fa-lab-2 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-10-25T23:59:59

CFIDDLE_INCLUDE=/cse142L/cfiddle/src/cfiddle/resources/include
CXX_STANDARD=-std=gnu++11
TARGET=x86_64-linux-gnu
MORE_LIBS=-pthread

include  /cse142L/cfiddle/src/cfiddle/resources/make/cfiddle.make
include /cse142L/cse141pp-archlab/cse141.make


$(BUILD)/run_tests.o: hidden_tests.cpp
$(BUILD)/run_tests.o: CXX_STANDARD=-std=gnu++17

run_tests.exe: $(BUILD)/run_tests.o $(BUILD)/matmul_solution.o $(BUILD)/matmul.o
	$(CXX) $^ $(LDFLAGS) -L$(GOOGLE_TEST_ROOT)/lib -lgtest -lgtest_main  -o $@

regressions.json: run_tests.exe
	./run_tests.exe --gtest_output=json:$@

autograde.csv: matmul_solution.cpp matmul.cpp canary.cpp
	./lab_lint.py matmul_solution.cpp
	./run_bench.py --results $@ --source matmul_solution.cpp --optimize "-O0"
	rm -rf build run_tests.exe

.PHONY: clean
clean:
	rm -rf regressions.json autograde.csv run_tests.exe
	rm -rf build

.PHONY: deep-clean
deep-clean: clean
	rm -rf .cfiddle



.PHONY: autograde
autograde: regressions.json autograde.csv

