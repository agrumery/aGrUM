import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.WeightedSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
sorted(ie.hardEvidenceNodes())
# [0, 2]
