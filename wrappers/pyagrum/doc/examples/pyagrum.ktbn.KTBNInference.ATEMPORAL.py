import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8, 0.5, 0.5, 0.5, 0.5])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
model.cpt("C", ie.ATEMPORAL).fillWith([0.3, 0.7])
ie.ATEMPORAL
# -1
ie.makeInference(3)
print(ie.posterior("C", ie.ATEMPORAL))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.3000  │ 0.7000  │
