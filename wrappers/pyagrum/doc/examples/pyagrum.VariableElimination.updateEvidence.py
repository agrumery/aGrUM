import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.setEvidence({"A": 0, "B": 1})
ie.updateEvidence({"A": 1, "C": [0.2, 0.9]})  # B is kept, A is changed, C is added
ie.nbrEvidence()
# 3
sorted(ie.softEvidenceNodes())
# [2]
