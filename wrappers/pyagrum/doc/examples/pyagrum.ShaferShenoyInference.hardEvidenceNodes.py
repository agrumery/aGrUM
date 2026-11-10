import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.setEvidence({"A": 1, "B": 0, "C": [0.2, 0.9]})
sorted(ie.hardEvidenceNodes())
# [0, 1]
