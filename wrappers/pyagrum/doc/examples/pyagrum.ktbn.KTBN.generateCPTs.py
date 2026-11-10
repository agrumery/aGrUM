import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.addTemporal(gum.LabelizedVariable("X", "X", 2))
model.addArc("X", 0, "X", 1)
model.generateCPTs()
round(model.cpt("X", 0).sum(), 4)
# 1.0
round(model.cpt("X", 1).sum(), 4)  # one distribution per value of X[0]
# 2.0
