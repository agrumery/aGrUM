# requires: pandas, IPython
import pyagrum.clg as gclg
import pyagrum.clg.notebook as gclgnb

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
g = gclgnb.getInference(clg, evs={"C": 7.0})  # a pydot graph
type(g).__name__
# 'Dot'
