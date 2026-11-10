# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
clg.dag2dict()
# {0: {1}, 1: {2}, 2: set()}
