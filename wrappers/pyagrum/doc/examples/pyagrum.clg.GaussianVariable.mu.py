# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

v = gclg.GaussianVariable("X", 1.5, 0.3)
v.mu()
# 1.5
