import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.LoopyWeightedSampling(bn)
ie.addEvidence("A", 1)  # sampling only accepts hard evidence
ie.hasSoftEvidence("A")
# False
