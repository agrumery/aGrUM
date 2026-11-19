import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.MonteCarloSampling(bn)
ie.addEvidence("A", 1)
ie.hasEvidence("A"), ie.hasEvidence("B")
# (True, False)
