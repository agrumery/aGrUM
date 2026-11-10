import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.3100  │ 0.6900  │
ie.setEvidence({"A": 0})
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.5500  │ 0.4500  │
