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
        
    compiled = build(source, arg_map(OPTIMIZE=[f"-{optimize}"], MORE_SRC="nibble.cpp canary.cpp"))
    with local_execution():
        results = run(compiled, function=["run_nibble","_canary"], 
                      arguments=arg_map(target_count=800000000, seed=seed))
    results.as_csv(results_file)
    
if __name__== "__main__":
    run_bench()
