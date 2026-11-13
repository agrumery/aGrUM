# requires: pandas, scipy, sklearn
import pyagrum as gum
import pyagrum.clg as gclg

gum.initRandom(42)  # the draws of pyagrum.clg come from the random generator of aGrUM
clg = gclg.SEM.toclg("""
A = 0[1]
B = 2A[1]
C = -1.5B[1]
""")
gclg.ForwardSampling(clg).makeSample(2000).tocsv("data.csv")
learner = gclg.CLGLearner("data.csv")
learner.Pearson_coeff(0, 1, set())  # stored in learner.r_XYZ
r = learner.r_XYZ[(frozenset({0, 1}), frozenset())]
round(float(r.mean()), 2)  # the correlation between A and B
# 0.9
