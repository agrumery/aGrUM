# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
print(clg.variable("B"))  # name:mu[sigma]
# B:1.0[2.0]
clg.variable(0).name()
# 'A'
