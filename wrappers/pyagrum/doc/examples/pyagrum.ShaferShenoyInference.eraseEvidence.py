import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.eraseEvidence("A")
ie.hasEvidence("A")
# False
ie.hasEvidence("C")
# True
