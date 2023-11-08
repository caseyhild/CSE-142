#!/usr/bin/env python

import click
from cfiddle import *
from CSE142L.notebook import setup_lab, local_execution
from notebook import *
from encryption import decrypt_bytes
from pandas.api.types import is_numeric_dtype
import numpy as np
import pandas as pd

class CanaryException(Exception):
    pass

@click.command()
@click.option("--results", "results_file", required=True, help="Where to put results.")
#@click.option("--source", required=True, help="Solution source code")
#@click.option("--optimize", required=True, help="Optimization flags")
def run_bench(results_file=None,
              source=None,
              optimize=None):
    return do_run_bench(results_file, build_options=None, run_locally=True)

def do_run_bench(results_file, build_options, run_locally):

    if build_options is None:
        build_options = dict()
        
    try:
        with open ("autograde_seed.encrypted", "rb") as s:
            seed = int(decrypt_bytes(s.read()).decode())
    except:
        seed = 42


    matexp = build("matexp.cpp",
                   arg_map(OPTIMIZE="-O3 -fopenmp -march=native", MORE_SRC="canary.cpp", **LAB_BUILD_PARAMETERS, **build_options),
                   rebuild=True)        

    def run_experiments():
        results = run(matexp, "bench_solution", arg_map(threads=12, seed=seed, iterations=1, p1=1, p2=1, p3=1, p4=1, p5=1),
                      run_options=arg_map(REP=range(0,4)))
        canary = run(matexp, "_canary",
                      arguments=arg_map(threads=6),
                      run_options=arg_map(MHz=3500))
        return (results+canary).as_df()

    if run_locally:
        with local_execution():
            data = run_experiments()
    else:
        data = run_experiments()



    def mean_str(col):
        #https://stackoverflow.com/a/52142310/3949036
        if is_numeric_dtype(col):
            return col.mean()
        else:
            return col.unique()[0] if col.nunique() == 1 else np.NaN


    data["benchmark"] = data["tag"]
    if "MORE_CFLAGS" in data:
        data["solution"] = data["MORE_CFLAGS"].apply(lambda x: "baseline" if x == "-DSTUDENT_SOLUTION" else "fast_version")
    else:
        data["solution"] = "student_solution"
        
    data = data.groupby(["solution", "tag", "function"]).agg(mean_str)
    data = format_data(data)
    data.to_csv(results_file)
    return data

def format_data(df):
    return pd.pivot_table(df, index="benchmark", values="ET", columns="solution")

if __name__== "__main__":
    run_bench()
