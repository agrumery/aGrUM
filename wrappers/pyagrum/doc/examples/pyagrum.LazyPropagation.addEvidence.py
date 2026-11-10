import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
ie.addEvidence("A", 1)  # hard evidence: A takes its value of index 1
ie.addEvidence("C", [0.2, 0.9])  # soft evidence: a likelihood on C
ie.nbrEvidence()
# 2
print(ie.posterior("B"))
#
#   B                │
# 0        │1        │
# ─────────│─────────│
#  0.1986  │ 0.8014  │
