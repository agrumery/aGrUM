# requires: pandas, scipy, sklearn, matplotlib, IPython, graphviz
import pyagrum.clg as gclg
import pyagrum.clg.notebook as gclgnb

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
import os

gclgnb.exportInference(clg, "inference.svg", evs={"C": 7.0})
os.path.exists("inference.svg")
# True
