# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

list(gclg.CLGLearner.generate_XYZ([0, 1, 2]))
# [(0, 1, set()), (0, 1, {2}), (0, 2, set()), (0, 2, {1}), (1, 2, set()), (1, 2, {0})]
