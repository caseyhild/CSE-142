#!/usr/bin/env python

import click
from cfiddle import *
from CSE142L.notebook import setup_lab, local_execution
from encryption import decrypt_bytes
class CanaryException(Exception):
    pass

@click.command()
@click.option("--results", "results_file", required=True, help="Where to put results.")
@click.option("--source", required=True, help="Solution source code")
@click.option("--optimize", required=True, help="Optimization flags")
def run_bench(results_file=None,
              source=None,
              optimize=None):

    try:
        with open ("autograde_seed.encrypted", "rb") as s:
            seed = int(decrypt_bytes(s.read()).decode())
    except:
        seed = 42

    alloc = build(source, build_parameters=arg_map(OPTIMIZE=[optimize], MORE_SRC="canary.cpp Allocator.cpp ChunkAlloc.cpp"), rebuild=True)

    to_run = arg_map(executable=alloc,
                     function=["allocator_bench_solution"], 
                     arguments=arg_map(count=128*1024*16, seed=seed),
                     run_options=arg_map(MHz=3500))
    
    to_run += arg_map(executable=alloc, function=["allocator_microbench_solution"], 
                      arguments=arg_map(count=1600000, seed=seed),
                      run_options=arg_map(MHz=3500))
    
    to_run += arg_map(executable=alloc, function=["miss_machine_solution"], 
                      arguments=arg_map(link_count=4096, access_count=100000000, seed=seed),
                      run_options=arg_map(MHz=3500))
    
    to_run += arg_map(executable=alloc, function="_canary",
                      arguments=arg_map(),
                      run_options=arg_map(MHz=3500))
    
    with local_execution():
        results = run_list(to_run)
        
    results.as_csv(results_file)
    
if __name__== "__main__":
    run_bench()
