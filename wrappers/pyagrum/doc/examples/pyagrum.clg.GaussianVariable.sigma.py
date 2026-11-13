# requires: pandas
import pyagrum.clg as gclg

v = gclg.GaussianVariable("X", 1.5, 0.3)
v.sigma()
# 0.3
