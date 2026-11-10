import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.setEvidence({"A": 1, "C": [0.2, 0.9]})
ie.nbrEvidence()
# 2
