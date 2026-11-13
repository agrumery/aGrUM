# requires: pandas
import pyagrum.clg as gclg

clg = gclg.CLG()
clg.add(gclg.GaussianVariable("A", 2, 1))
# 0
clg.add(gclg.GaussianVariable("B", 1, 2))
# 1
clg.names()
# ['A', 'B']
