import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyMonteCarloSampling(bn)
ie.addEvidence("A", 1)
ie.hasHardEvidence("A"), ie.hasHardEvidence("C")
# (True, False)
