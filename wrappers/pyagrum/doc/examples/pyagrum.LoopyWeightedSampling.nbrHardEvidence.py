import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.nbrHardEvidence()
# 2
