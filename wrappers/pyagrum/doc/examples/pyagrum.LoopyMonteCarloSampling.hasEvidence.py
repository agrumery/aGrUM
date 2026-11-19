import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyMonteCarloSampling(bn)
ie.addEvidence("A", 1)
ie.hasEvidence("A"), ie.hasEvidence("B")
# (True, False)
