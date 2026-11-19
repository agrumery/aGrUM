import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyGibbsSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.nbrEvidence()
# 2
ie.eraseAllEvidence()
ie.nbrEvidence()
# 0
