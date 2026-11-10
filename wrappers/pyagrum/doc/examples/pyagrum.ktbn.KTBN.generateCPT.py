import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN(2)
model.addTemporal(gum.LabelizedVariable("X", "X", 2))
model.addArc("X", 0, "X", 1)
model.generateCPT("X", 1)
[round(v, 6) for v in model.cpt("X", 1).sumOut("X[1]").tolist()]  # each distribution sums to 1
# [1.0, 1.0]
