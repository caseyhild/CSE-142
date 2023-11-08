SHELL=/bin/bash
.SUFFIXES:
default:

.PHONY: create-labs
create-labs:
	cse142 lab delete -f caches2-bench
	cse142 lab delete -f caches2
	cse142 lab create --name "Lab 4: Caches II (Benchmark)" --short-name "caches2-bench" --docker-image stevenjswanson/cse142l-runner:22fa-lab-4 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-11-23T23:59:59 --starter-repo https://github.com/CSE142/fa22-CSE142L-caches2-starter.git --starter-branch main
	cse142 lab create --name "Lab 4: Caches II" --short-name "caches2" --docker-image stevenjswanson/cse142l-runner:22fa-lab-4 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-11-23T23:59:59



NB_RELEASE_INCLUDES=/cse142L/labs/CSE141pp-Lab-Common -I .

ENCRYPTED_FILES=hidden_tests.cpp autograde_seed
STUDENT_EDITABLE_FILES=AlignedAllocator.hpp
PRIVATE_FILES=Lab.key.ipynb admin .git solution bad-solution Allocator_PA.key.ipynb Moneta_Demos.ipynb Demos.ipynb example $(ENCRYPTED_FILES)  

include aligned.makefile
