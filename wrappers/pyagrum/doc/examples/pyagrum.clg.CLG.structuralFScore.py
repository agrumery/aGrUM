# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
other = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + A[1]
""")
clg.structuralFScore(clg)
# 1.0
clg.structuralFScore(other)
# 0.5
