import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.setEvidence({"A": 1})  # sampling only accepts hard evidence
ie.softEvidenceNodes()
# set()
