import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.addIntervention("X", 1, "1")  # do(X[1] = 1), by label (or by index: 1)
ie.makeInference(3)
print(ie.posterior("X", 2))
#
#   X[2]             │
# 0        │1        │
# ─────────│─────────│
#  0.2000  │ 0.8000  │
print(ie.posterior("X", 0))  # no effect on the past
#
#   X[0]             │
# 0        │1        │
# ─────────│─────────│
#  0.5000  │ 0.5000  │
ie.addIntervention({"X[1]": 0, ("X", 2): 0})  # batch, engine names or (base, slice)
ie.makeInference(3)
ie.posterior("X", 2).tolist()
# [1.0, 0.0]
