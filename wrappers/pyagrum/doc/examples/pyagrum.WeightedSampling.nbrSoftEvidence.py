import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.setEvidence({"A": 1})  # sampling only accepts hard evidence
ie.nbrSoftEvidence()
# 0
