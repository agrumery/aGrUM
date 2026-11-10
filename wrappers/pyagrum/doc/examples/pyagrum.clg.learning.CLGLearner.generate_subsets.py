# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

list(gclg.CLGLearner.generate_subsets({0, 1, 2}))
# [set(), {0}, {1}, {2}, {0, 1}, {0, 2}, {1, 2}, {0, 1, 2}]
