# requires: pandas, scipy, sklearn
import numpy as np

import pyagrum.clg as gclg

clg = gclg.SEM.toclg("""
A = 0[1]
B = 2A[1]
C = -1.5B[1]
""")
gclg.ForwardSampling(clg).makeSample(500, seed=42).tocsv("data.csv")
np.random.seed(42)  # the learner uses the random generator of numpy
learner = gclg.CLGLearner("data.csv")
C = {0: {1, 2}, 1: {0, 2}, 2: {0, 1}}  # the complete skeleton
learner.Repeat_II([0, 1, 2], C, 1)  # tests with separating sets of size 1
# True
C
# {0: {1}, 1: {0, 2}, 2: {1}}
