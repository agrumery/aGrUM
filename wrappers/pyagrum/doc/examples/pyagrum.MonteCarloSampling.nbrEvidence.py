import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.nbrEvidence()
# 2
