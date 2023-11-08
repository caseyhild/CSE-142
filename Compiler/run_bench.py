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
        
    with local_execution():
        matmul = build(source, build_parameters=arg_map(OPTIMIZE=[optimize], MORE_SRC="canary.cpp matmul.cpp"))
        results = run(matmul, function=[f"test_matrix_product_{k}" for k in range(1,6)] + ["_canary"], arguments=arg_map(rows=500, size1=550, columns=450, size2=475, log_size=9, seed=seed))

    results.as_csv(results_file)
    
if __name__== "__main__":
    run_bench()
