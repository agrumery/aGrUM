# requires: pandas
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
C = {0: {1, 2}, 1: {0, 2}, 2: {0, 1}}  # the complete skeleton
learner.Repeat_II([0, 1, 2], C, 1)  # tests with separating sets of size 1
# True
C
# {0: {1}, 1: {0, 2}, 2: {1}}
