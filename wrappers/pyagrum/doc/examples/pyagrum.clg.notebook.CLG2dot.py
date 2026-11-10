# requires: pandas, scipy, sklearn, matplotlib, IPython
import pyagrum.clg as gclg
import pyagrum.clg.notebook as gclgnb

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
g = gclgnb.CLG2dot(clg)  # a pydot graph
sorted(n.get_name().strip('"') for n in g.get_nodes() if n.get_name() not in ("node", "edge", "graph"))
# ['A', 'B', 'C']
