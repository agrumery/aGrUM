# requires: pandas
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
bn = clg.asDiscreteBN(3)  # same structure, discrete variables
sorted(bn.names())
# ['A', 'B', 'C']
bn.variable("A").domainSize()
# 3
