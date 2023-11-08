from cfiddle import *
from cfiddle.config import set_config
import os

#pa_columns=["function", "seed", "thread", "size", "power","p1", "p2", "p3", "p4", "p5", "IC", "CPI", "CT", "ET", "L1_MPI", "TLB_MPI", "L1_cache_misses", "TLB_misses"]

#hist_columns =["function", "thread", "size", "arg1", "arg2", "arg3", "IC", "CPI", "ET", "L1_cache_misses", "L1_MPI"]

default_layers=["misses-compulsory-all", "misses-capacity-all", "hits-all"]
os.environ["OPENMP"] = "yes"

LAB_BUILD_PARAMETERS=dict(MORE_INCLUDES=f"-I{os.environ['MONETA_ROOT']}/moneta/", MORE_LIBS="-lgomp")

