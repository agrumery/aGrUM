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
skeleton, sepset = learner.Adjacency_search([0, 1, 2])
skeleton  # A-B-C: A and C are separated
# {0: {1}, 1: {0, 2}, 2: {1}}
sepset[(0, 2)]  # ... by B
# {1}
