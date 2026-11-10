# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(1000, seed=42)
round(float(fs.mean_sample("C")), 1)  # theoretical mean: 3 + (1 + 0.5*2) = 5
# 4.9
