from IPython.display import  HTML
from CSE142L.notebook import compare
from cfiddle.jupyter.util import html_parameters
from cfiddle import *

def side_by_side(function, *argc, **kwargs):
    data = render_czoo("czoo", function, *argc, **kwargs)
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Source</span></div>"))
    display(data['opt'].source)    
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Unoptimized</span></div>"))
    compare([data['unopt'].asm, data['unopt'].cfg])
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Optimized</span></div>"))
    compare([data['opt'].asm, data['opt'].cfg])

def stacked(function, *argc, **kwargs):
    data = render_czoo("czoo", function, *argc, **kwargs)
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Source</span></div>"))
    display(data['opt'].source)    
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Unoptimized</span></div>"))
    display(data['unopt'].asm)
    display(data['unopt'].cfg)
    display(HTML("<div style='text-align:center; font-weight: bold'><span>Optimized</span></div>"))
    display(data['opt'].asm)
    display(data['opt'].cfg)


def src_vs_asm(exe, function):
    compare([exe.source(function),exe.asm(function)])

def analyze(exe, function, **kwargs):
    compare([exe.source(function),exe.asm(function)])
    display(exe.cfg(function, **kwargs))

def optimization_example(code, function, cfg=False, *argc, OPTIMIZE=None, DEBUG_FLAGS=None, **kwargs):
    if OPTIMIZE is None:
        OPTIMIZE = ["-O0", "-O1"]

    DEBUG_FLAGS = "-g0"

    foo = build(code, arg_map(OPTIMIZE=[f"{x} {DEBUG_FLAGS}" for x in OPTIMIZE]), **kwargs)
    display(foo[0].source())
    compare([x.asm(function) for x in foo], [html_parameters(v.get_build_parameters()) for v in foo])
    
    if cfg:
        compare([x.cfg(function, number_nodes=True) for x in foo], [html_parameters(v.get_build_parameters()) for v in foo])


compare_style=r"""
            <style>
        .side-by-side {
            display: flex;
            align-items: stretch;
        }
        .side-by-side-pane {
            margin-right:1em;
            border-right-style: solid;
            border-right-color: black;
            border-right-width: 1px;
            flex: 1;
        }
        .side-by-side-pane .heading{
            font-size: 1.5;
            font-weight: bold;
            text-align:center;
            border-bottom-style: dotted;
            border-bottom-width: 1px;
            border-bottom-color: gray;
            margin-left: 1em;
            margin-right: 1em;

        }
        </style>
"""



def heading(s):
    display(HTML(f"""
    {compare_style}
        <div class="side-by-side"> """ +
                 
                 f"<div class='side-by-side-pane'><div class='heading'>{s}</div></div>" +


                 """
        </div>
    """))


            
def incremental_average(d):
    return [d[0:i+1].mean() for i in range(len(d))]

