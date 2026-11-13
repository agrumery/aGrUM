# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
sorted(clg.arcs())
# [(0, 1), (1, 2)]
print(clg.variable("B"))
# B:1.0[2.0]
