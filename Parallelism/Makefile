SHELL=/bin/bash
.SUFFIXES:
default:

.PHONY: create-labs
create-labs:
	cse142 lab delete -f parallel-bench
	cse142 lab delete -f parallel
	cse142 lab create --name "Lab 5: Parallelism (Benchmark)" --short-name "parallel-bench" --docker-image stevenjswanson/cse142l-runner:22fa-lab-5 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-12-08T23:59:59 --starter-repo https://github.com/CSE142/fa22-CSE142L-parallel-starter.git --starter-branch main
	cse142 lab create --name "Lab 5: Parallelism" --short-name "parallel" --docker-image stevenjswanson/cse142l-runner:22fa-lab-5 --execution-time-limit 0:05:00 --total-time-limit 1:00:00 --due-date 2022-12-08T23:59:59

ENCRYPTED_FILES=hidden_tests.cpp autograde_seed
PRIVATE_FILES=Lab.key.ipynb admin .git solution solutions bad-solution PA_matexp matexp_solutions $(ENCRYPTED_FILES)  matexp.ipynb
NB_RELEASE_INCLUDES:=$(NB_RELEASE_INCLUDES) -I .

include matexp.make


