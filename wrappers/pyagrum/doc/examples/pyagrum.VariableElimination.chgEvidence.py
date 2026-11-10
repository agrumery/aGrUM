import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.VariableElimination(bn)
ie.addEvidence("A", 0)
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.5500  │ 0.4500  │
ie.chgEvidence("A", 1)
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.2500  │ 0.7500  │
