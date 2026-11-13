# requires: pandas
import pyagrum.clg as gclg

clg = gclg.CLG()
clg.add(gclg.GaussianVariable("A", 2, 1))
clg.add(gclg.GaussianVariable("B", 1, 2))
clg.addArc("A", "B", 0.5)  # B = 1 + 0.5*A + noise
# (0, 1)
clg.coefArc("A", "B")
# 0.5
