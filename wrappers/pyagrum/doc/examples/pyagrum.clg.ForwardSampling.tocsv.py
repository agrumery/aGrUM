# requires: pandas, scipy, sklearn
import pyagrum as gum
import pyagrum.clg as gclg

gum.initRandom(42)  # the draws of pyagrum.clg come from the random generator of aGrUM
clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
fs = gclg.ForwardSampling(clg).makeSample(100)
fs.tocsv("sample.csv")
open("sample.csv").readline().strip()
# 'A,B,C'
