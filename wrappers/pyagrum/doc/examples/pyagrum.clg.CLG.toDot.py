# requires: pandas, IPython
import pyagrum.clg as gclg
import pyagrum.clg.notebook  # adds toDot to CLG

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
dot = clg.toDot()
dot.startswith("digraph")
# True
'"A"->"B"' in dot.replace(" ", "")
# True
