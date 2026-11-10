import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.setEvidence({"A": 1, "B": "0", "C": 0})
ie.nbrHardEvidence()
# 3
