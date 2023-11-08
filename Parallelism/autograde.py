#!/usr/bin/env python

import click
import os
import json
import re
from csvtools import qcsv
from CSE142L.jextract import extract as qjson
from functools import reduce
import pandas as pd

        
def compute_scores(df, targets):
    assert df.index.equals(targets.index)
    
    scores = df.copy()

    scores["target_speedup"] = targets["target_speedup"]
    scores["baseline"] = targets["baseline"]
    scores["weight"] = targets["weight"]

    scores['speedup'] = scores["baseline"]/scores["student_solution"]

    scores['raw_score'] = scores['speedup']/scores['target_speedup'] * 100.0 * scores['weight']
    scores['max_points'] = scores["weight"] * 100.0
    scores['score'] =  scores[['raw_score','max_points']].min(axis=1)
    
    return scores[['benchmark',
                   'target_speedup', "baseline", "student_solution", 'speedup', 'weight','raw_score', 'score']]

class CanaryException(Exception):
    pass

def check_canary(bench):
    canary = bench[bench["benchmark"] == "canary"]

    if canary["student_solution"].iloc[0] >1.2:
        raise CanaryException(f"your canary ran too slowly ({canary['student_solution'].iloc[0]}).  Rerun your code")
    
def compute_all_scores(results, targets):
    def csv(f):
        return pd.read_csv(f, sep=",")

    bench = csv(results)

    check_canary(bench);
    
    targets = csv(targets)
    
    scores = compute_scores(bench, targets)

    return scores.copy()
    

@click.command()
@click.option("--submission", required=True,  type=click.Path(exists=True), help="Test directory.")
@click.option("--results", required=True, type = click.File(mode="w"), help="Where to put results.")
@click.option("--targets", required=True, help="Performance targets file")
@click.option("--ignore-regressions", is_flag=True, default=False,  help="Don't look at regressions")
@click.option("--scores", "scores_csv", default="scores.csv", type=click.File("bw"), help="Where to put scores csv.")
def autograde(submission=None,
              targets=None,
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
        scores  = compute_all_scores(os.path.join(submission, "autograde.csv"), targets)
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
        raise 

        benchmarks.append(dict(score=0,
                               max_score=100,
                               output = f"I got an unexpected exception evaluating the benchmarks.    Tell the course staff:{type(e).__name__}: {e}.",
                               tags=(),
                               visibility="visible"))
    else:
        for index, row in scores.iterrows():
            max_score = round(100.0*row['weight'],2)
            benchmarks.append(dict(score=round(row['score'],2) if failures == 0 else 0,
                                   max_score = max_score,
                                   output=f"Test: {row['benchmark']}:  The target speedup is {row['target_speedup']:2.2f}x, your speedup is {row['speedup']:2.2f}x.  Your score is {row['speedup']:2.2f}/{row['target_speedup']:2.2f}*{max_score:2.2f} = {row['raw_score']:2.2f} (or {max_score:2.2f}, if that values is greater than {max_score:2.2f})" if failures == 0 else "Your code is incorrect, so speedup is meaningless.",
                                   tags=[],
                                   visibility="visible"))

        leaderboard = []
        if failures == 0:
            for index, row in scores.iterrows():
                if "_canary" not in row['benchmark']:
                    leaderboard.append(dict(name=row['benchmark'] + " speedup", value=round(row['speedup'],2)))
            leaderboard.append(dict(name="Speedup Sum", value=round(sum(scores['speedup']),2)))
            #leaderboard.append(dict(name="Speedup Product", value=round(reduce(lambda x,y: x*y, scores['speedup'],1),2)))
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
