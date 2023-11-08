import os
import pandas


os.environ['PIN_ROOT'] = "/cse142L/CSE141pp-Tool-Moneta-Pin/"

pandas.set_option('display.max_rows', None)


LAB_BUILD_PARAMETERS=dict(MORE_INCLUDES=f"-I{os.environ['MONETA_ROOT']}/moneta/")


