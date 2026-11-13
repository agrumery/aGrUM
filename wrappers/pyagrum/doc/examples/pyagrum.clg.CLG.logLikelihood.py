# requires: pandas
import pyagrum as gum
import pyagrum.clg as gclg

gum.initRandom(42)  # the draws of pyagrum.clg come from the random generator of aGrUM
clg = gclg.SEM.toclg("""
A = 2[1]
B = 1 + 0.5A[2]
C = 3 + B[1]
""")
gclg.ForwardSampling(clg).makeSample(500).tocsv("data.csv")
ll = clg.logLikelihood("data.csv")
bool(ll < 0)  # a log-likelihood of continuous data is not bounded, but here negative
# True
