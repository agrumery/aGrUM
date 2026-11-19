import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.WeightedSampling(bn)
ie.addEvidence("A", 1)
ie.makeInference()
ie.H("A") == 0  # A is observed
# True
ie.H("B") > 0
# True
