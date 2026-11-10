import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.setEvidence({"A": 1, "C": [0.2, 0.9]})
ie.hasSoftEvidence("C")
# True
ie.hasSoftEvidence("A")
# False
