# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(1000, seed=42)
round(float(fs.variance_sample("B")), 1)  # theoretical variance: 0.5**2 * 1 + 2**2 = 4.25
# 4.3
