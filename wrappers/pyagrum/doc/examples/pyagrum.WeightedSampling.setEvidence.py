import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.WeightedSampling(bn)
ie.setEvidence({"A": 1, "C": "0"})  # index or label
sorted(ie.hardEvidenceNodes())
# [0, 2]
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
