import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.setEvidence({"A": 1})
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
round(ie.posterior("B").sum(), 4)
# 1.0
