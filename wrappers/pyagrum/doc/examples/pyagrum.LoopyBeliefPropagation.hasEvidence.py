import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 1)
ie.hasEvidence("A"), ie.hasEvidence("B")
# (True, False)
