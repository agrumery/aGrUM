import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyImportanceSampling(bn)
ie.addEvidence("A", 1)
ie.hasEvidence("A"), ie.hasEvidence("B")
# (True, False)
