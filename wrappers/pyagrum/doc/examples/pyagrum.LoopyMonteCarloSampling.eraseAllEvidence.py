import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyMonteCarloSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.eraseAllEvidence()
ie.nbrEvidence()
# 0
