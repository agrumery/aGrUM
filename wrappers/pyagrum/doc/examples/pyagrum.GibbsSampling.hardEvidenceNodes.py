import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.addEvidence("A", 1)
ie.addEvidence("C", 0)
sorted(ie.hardEvidenceNodes())
# [0, 2]
