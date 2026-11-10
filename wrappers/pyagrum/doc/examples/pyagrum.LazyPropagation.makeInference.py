import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
ie.setEvidence({"C": 1})
ie.makeInference()
print(ie.posterior("A"))
#
#   A                │
# 0        │1        │
# ─────────│─────────│
#  0.1304  │ 0.8696  │
