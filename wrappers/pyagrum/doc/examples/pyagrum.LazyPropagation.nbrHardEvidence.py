import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.setEvidence({"A": 1, "B": 0, "C": [0.2, 0.9]})
ie.nbrHardEvidence()
# 2
