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
learner.test_indep(0, 2, set())  # A and C are dependent
# False
learner.test_indep(0, 2, {1})  # but independent given B
# True
