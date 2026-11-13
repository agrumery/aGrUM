# requires: pandas
import pyagrum as gum
import pyagrum.clg as gclg

gum.initRandom(42)  # the draws of pyagrum.clg come from the random generator of aGrUM
clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(1000)
round(float(fs.mean_sample("C")), 1)  # theoretical mean: 3 + (1 + 0.5*2) = 5
# 5.1
