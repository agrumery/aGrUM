# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(1000, seed=42)
fs.covariance_sample(["A", "B"]).shape
# (2, 2)
fs.covariance_sample(["A", "B"]).round(1)
# array([[1. , 0.5],
#        [0.5, 4.3]])
