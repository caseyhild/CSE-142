.SUFFIXES: 
default: 

ENCRYPTED_FILES=hidden_tests.cpp
PRIVATE_FILES=Lab.key.ipynb admin .git Moneta.md nibble_fast.cpp nibble_baseline.cpp $(ENCRYPTED_FILES)

STUDENT_EDITABLE_FILES=nibble_solution.cpp

DOCKER_IMAGE=stevenjswanson/cse142l-runner:22fa-lab-1
STARTER_REPO=https://github.com/CSE142/fa22-CSE142L-intro-starter.git

.PHONY: create-labs
create-labs:
	cse142 lab delete -f intro-bench
	cse142 lab create --name "Lab 1: The Performance Equation (Benchmark)" --short-name "intro-bench" --docker-image $(DOCKER_IMAGE) --starter-repo $(STARTER_REPO) --starter-branch main --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-10-11T23:59:59

	cse142 lab delete -f intro
	cse142 lab create --name "Lab 1: The Performance Equation" --short-name "intro" --docker-image $(DOCKER_IMAGE) --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-10-11T23:59:59

CFIDDLE_INCLUDE=/cse142L/cfiddle/src/cfiddle/resources/include
CXX_STANDARD=-std=gnu++17
TARGET=x86_64-linux-gnu
MORE_LIBS=-pthread

include  /cse142L/cfiddle/src/cfiddle/resources/make/cfiddle.make
include /cse142L/cse141pp-archlab/cse141.make

.PHONY: autograde
autograde: regressions.json autograde.csv

$(BUILD)/run_tests.o: hidden_tests.cpp

run_tests.exe: $(BUILD)/run_tests.o $(BUILD)/nibble_solution.o $(BUILD)/nibble.o
	$(CXX) $^ $(LDFLAGS) -L$(GOOGLE_TEST_ROOT)/lib -lgtest -lgtest_main  -o $@

regressions.json: run_tests.exe
	./run_tests.exe --gtest_output=json:$@

autograde.csv: nibble_solution.cpp nibble.cpp canary.cpp
	./run_bench.py --results $@ --source nibble_solution.cpp --optimize O4

.PHONY: clean
clean:
	rm -rf regressions.json autograde.csv run_tests.exe
	rm -rf build

.PHONY: deep-clean
deep-clean: clean
	rm -rf .cfiddle

