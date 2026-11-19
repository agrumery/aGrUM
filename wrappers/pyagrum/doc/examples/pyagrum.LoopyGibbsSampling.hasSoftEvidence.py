import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyGibbsSampling(bn)
ie.addEvidence("A", 1)
ie.addEvidence("C", 0)
ie.hasSoftEvidence("A"), ie.hasSoftEvidence("C")  # sampling accepts hard evidence only
# (False, False)
