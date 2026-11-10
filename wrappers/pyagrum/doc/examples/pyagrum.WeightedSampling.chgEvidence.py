import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.addEvidence("A", 0)
ie.chgEvidence("A", 1)
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
