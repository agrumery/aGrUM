# requires: pandas, IPython, graphviz
import pyagrum.clg as gclg
import pyagrum.clg.notebook as gclgnb

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
import os

gclgnb.exportCLG(clg, "clg.svg")
os.path.exists("clg.svg")
# True
