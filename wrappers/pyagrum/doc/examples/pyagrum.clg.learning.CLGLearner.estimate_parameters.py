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
mu, sigma, coef = learner.estimate_parameters({0: {1}, 1: {2}, 2: set()})
{arc: round(float(c), 1) for arc, c in coef.items()}
# {(0, 1): 2.0, (1, 2): -1.5}
