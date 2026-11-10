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
C = {0: {1}, 1: {0, 2}, 2: {1}}  # the skeleton A-B-C, no v-structure
learner.three_rules(C)  # nothing can be oriented
# {0: {1}, 1: {0, 2}, 2: {1}}
