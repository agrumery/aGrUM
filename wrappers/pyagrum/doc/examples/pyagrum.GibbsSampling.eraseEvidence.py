import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.GibbsSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.eraseEvidence("A")
sorted(ie.hardEvidenceNodes())
# [2]
