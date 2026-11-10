import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.hasEvidence("A")
# False
ie.setEvidence({"A": 1})
ie.hasEvidence("A")
# True
ie.hasEvidence("C")
# False
