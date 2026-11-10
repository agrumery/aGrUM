import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.makeInference(3)
print(ie.posterior("X", 2))
#
#   X[2]             │
# 0        │1        │
# ─────────│─────────│
#  0.5850  │ 0.4150  │
print(ie.posterior("X[1]"))
#
#   X[1]             │
# 0        │1        │
# ─────────│─────────│
#  0.5500  │ 0.4500  │
