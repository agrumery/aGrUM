import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.setEvidence({"A": 1, "C": [0.3, 0.9]})
ie.nbrEvidence()
# 2
ie.eraseAllEvidence()
ie.nbrEvidence()
# 0
