#!/usr/bin/env python


import click
import os
import json
import re
from csvtools import qcsv
from CSE142L.jextract import extract as qjson
from functools import reduce
import pandas as pd

        
def compute_scores(df, FOM, targets):
    (base_FOM, target_FOM, target_S, weight) = zip(*targets)
    
    scores = df.copy()
    baseFOM='reference'+"_"+FOM

    scores['target_speedup'] = target_S
    scores[baseFOM] = base_FOM
    scores['speedup'] = scores[baseFOM]/scores[FOM]
    scores['weight'] = weight
    scores['bench_score'] = scores['speedup']/scores['target_speedup'] * 100.0 * weight

    scores['max_points'] = scores["weight"] * 100.0
    scores['capped_score'] =  scores[['bench_score','max_points']].min(axis=1)
    scores['score'] =  scores['capped_score']
    
    return scores[['label', 'target_speedup', baseFOM, FOM, 'speedup', 'weight','bench_score', 'score']]


class CanaryException(Exception):
    pass

def compute_all_scores(dir=None):
    if dir == None:
        dir=""

    def csv(f):
        return pd.read_csv(f, sep=",")

    bench = csv(os.path.join(dir, "autograde.csv"))
    bench["label"] = bench["function"]
    canary = bench[bench["function"] == "_canary"]

    if canary["ET"].iloc[0] > 7:
        raise CanaryException(f"your canary ran too slowly ({canary['ET'].iloc[0]}).  Rerun your code")

# SOURCE_FILE                matmul_fast.cpp  matmul_solution.cpp      speedup
# tag              OPTIMIZE
# matrix_product_1 -O0              0.347587              1.66767     4.797849
# matrix_product_2 -O0              0.260293              1.42895     5.489775
# matrix_product_3 -O0              0.497634              2.30611     4.634149
# matrix_product_4 -O0              0.413007              2.30825     5.588888
# matrix_product_5 -O0              0.001585              2.30311  1453.277132

# 	label	        target_speedup	reference_ET	ET	speedup	        weight	bench_score	score
# 0	three_dimensions21.07	        2.34162	        1.66402 1.407207	0.2	1.335744	1.335744
# 1	one_dimensions	5.12	        2.33964	        1.41808	1.649865	0.2	6.444784	6.444784
# 2	log_dimension	4.35	        2.34704	        2.27146	1.033274	0.2	4.750684	4.750684
# 3	fixed_dimension	4.93	        1.43043       	2.28304	0.626546	0.2	2.541770	2.541770
# 4	fixed	        4.31	        1.66817 	2.27314	0.733862	0.2	3.405390	3.405390
# 5	canary	        1.00	        6.16000 	6.16508	0.999176	0.0	0.000000	0.000000

    baseline_ETs = [
        1.66767,
        1.42895,
        2.30611,
        2.30825,
        2.30311
    ]

    target_ETs = [
        0.347587,
        0.260293,
        0.497634,
        0.413007,
        0.1 #0.001585
    ]

    weights = [
        0.2,
        0.2,
        0.2,
        0.2,
        0.2        
    ]
    params = [(b, t, round(b/t*0.9, 2), w) for b, t, w in zip(baseline_ETs, target_ETs, weights)]

    params.append((6.16, 6.16, 1, 0))
    
    scores = compute_scores(bench, "ET", params)

    return scores.copy()
    

@click.command()
@click.option("--submission", required=True,  type=click.Path(exists=True), help="Test directory.")
@click.option("--results", required=True, type = click.File(mode="w"), help="Where to put results.")
@click.option("--ignore-regressions", is_flag=True, default=False,  help="Don't look at regressions")
@click.option("--scores", "scores_csv", default="scores.csv", type=click.File("bw"), help="Where to put scores csv.")
def autograde(submission=None,
              results=None,
              ignore_regressions = None,
              scores_csv=None):

    if not ignore_regressions:
        try:
            failures = qjson(
                json.load(open(os.path.join(submission, "regressions.json"))),
                ["testsuites", 0, "failures"])
            output = "tests passed" if failures == 0 else "Your code is incorrect"
        except FileNotFoundError as e:
            output = f"I couldn't find your regression outputs.  This often means your program generated a segfault :{e}."
            failures = 1
        except IndexError as e:
            output = f"I got an unexpected exception processing the regressions.  Tell the course staff:{type(e).__name__}: {e}."
            failures = 1
        finally:
            regressions = dict(score=1 if failures == 0 else 0,
                               max_score=1,
                               number="1",
                               output=output,
                               tags=[],
                               visibility="visible")
    else:
        failures = 0
        regressions = []

        
    benchmarks = []
    leaderboard=[]
    try:
        scores  = compute_all_scores(dir=submission)
        scores.to_csv(scores_csv)
    except FileNotFoundError as e:
        benchmarks.append(dict(score=0,
                               max_score=100,
                               output = f"I couldn't find a csv file.  This often means your program generated a segfault or failed the regressions :{e}.",
                               tags=(),
                               visibility="visible"))
    except CanaryException:
        benchmarks.append(dict(score=0,
                               max_score=100,
                               output = f"The machine you ran was slow, so your grade would be lower than it should be.  Please re-run.  If this happens repeatedly, let the staff know.  Sorry about this.  If this is close to the deadline, please fill out the emergency submission problem form: https://forms.gle/ThHjESfbZRqqztXUA",
                               tags=(),
                               visibility="visible"))
    except IndexError as e:
        benchmarks.append(dict(score=0,
                               max_score=100,
                               output = f"I got an unexpected exception evaluating the benchmarks.    Tell the course staff:{type(e).__name__}: {e}.",
                               tags=(),
                               visibility="visible"))
    else:
        count = len(scores)
        for index, row in scores.iterrows():
            max_score = round(100.0*row['weight'],2)
            benchmarks.append(dict(score=round(row['score'],2) if failures == 0 else 0,
                                   max_score = max_score,
                                   output=f"Test: {row['label']}:  The target speedup is {row['target_speedup']:2.2f}x, your speedup is {row['speedup']:2.2f}x.  Your score is {row['speedup']:2.2f}/{row['target_speedup']}*{max_score:2.2f} = {row['bench_score']:2.2f} (or {max_score:2.2f}, if that values is greater than {max_score:2.2f})" if failures == 0 else "Your code is incorrect, so speedup is meaningless.",
                                   tags=[],
                                   visibility="visible"))

        leaderboard = []
        if failures == 0:
            for index, row in scores.iterrows():

                if "_canary" not in row['label']:
                    leaderboard.append(dict(name=row['label'] + " speedup", value=round(row['speedup'],2)))

    if os.path.exists("/autograder/results/stdout"):
        with open("/autograder/results/stdout") as f:
            stdout = f.read()
    else:
        stdout = ""
        
    # https://gradescope-autograders.readthedocs.io/en/latest/specs/#output-format
    json.dump(dict(output=stdout,
                   visibility="visible",
                   stdout_visibility="visible",
                   tests=[regressions] + benchmarks,
                   leaderboard=leaderboard),
              results, indent=4)
        
if __name__== "__main__":
    autograde()
