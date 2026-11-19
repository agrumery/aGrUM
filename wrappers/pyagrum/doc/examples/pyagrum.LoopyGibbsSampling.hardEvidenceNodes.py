import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyGibbsSampling(bn)
ie.addEvidence("A", 1)
ie.addEvidence("C", 0)
sorted(ie.hardEvidenceNodes())
# [0, 2]
