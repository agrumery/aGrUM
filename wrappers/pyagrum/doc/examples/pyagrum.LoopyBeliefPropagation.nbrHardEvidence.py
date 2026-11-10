import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.setEvidence({"A": 1, "B": "0", "C": [0.3, 0.9]})
ie.nbrHardEvidence()
# 2
