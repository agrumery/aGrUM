import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.VariableElimination(bn)
# hard evidence (index or label) and soft evidence (likelihood)
ie.setEvidence({"A": 1, "C": [0.2, 0.9]})
print(ie.posterior("B"))
#
#   B                │
# 0        │1        │
# ─────────│─────────│
#  0.1986  │ 0.8014  │
ie.setEvidence({"B": "0"})  # replaces all the previous evidence
sorted(ie.hardEvidenceNodes())
# [1]
