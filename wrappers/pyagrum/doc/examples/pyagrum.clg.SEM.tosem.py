# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
print(gclg.SEM.tosem(clg))
# A=2.0[1.0]
# B=1.0+0.5*A[2.0]
# C=3.0+B[1.0]
