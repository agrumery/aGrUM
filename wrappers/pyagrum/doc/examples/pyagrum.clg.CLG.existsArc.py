# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
clg.existsArc("A", "B")
# True
clg.existsArc("A", "C")
# False
