import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.addObservation("X", 1, 1)  # X[1] = 1
ie.makeInference(3)
print(ie.posterior("X", 0))  # the past is revised
#
#   X[0]             │
# 0        │1        │
# ─────────│─────────│
#  0.1111  │ 0.8889  │
ie.addObservation("X[1]", [0.2, 0.8])  # a soft evidence
print(ie.posterior("X", 0))
#
#   X[0]             │
# 0        │1        │
# ─────────│─────────│
#  0.2766  │ 0.7234  │
