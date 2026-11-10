# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(100, seed=42)
fs.toarray().shape  # one column per variable
# (100, 3)
fs.toarray("A").shape
# (100,)
