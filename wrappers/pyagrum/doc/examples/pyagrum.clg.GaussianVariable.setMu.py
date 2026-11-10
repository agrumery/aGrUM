# requires: pandas, scipy, sklearn
import pyagrum.clg as gclg

v = gclg.GaussianVariable("X", 1.5, 0.3)
v.setMu(-2)
v.mu()
# -2
