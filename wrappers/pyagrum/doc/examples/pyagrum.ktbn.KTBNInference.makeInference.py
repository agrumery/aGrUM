import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.makeInference(10)  # slices 0..9
len(ie.posteriors("X"))
# 10
print(ie.posterior("X", 9))
#
#   X[9]             │
# 0        │1        │
# ─────────│─────────│
#  0.6599  │ 0.3401  │
