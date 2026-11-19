import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.GibbsSampling(bn)
ie.addEvidence("A", 1)
ie.addEvidence("C", 0)
ie.hasHardEvidence("A"), ie.hasHardEvidence("C")
# (True, True)
