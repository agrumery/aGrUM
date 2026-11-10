import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.addEvidence("A", 1)
ie.makeInference()
ie.H("A") == 0  # A is observed
# True
ie.H("B") > 0
# True
