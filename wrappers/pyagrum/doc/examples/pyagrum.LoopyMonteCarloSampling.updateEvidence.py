import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.setEvidence({"A": 0})
ie.updateEvidence({"A": 1, "C": 0})  # changes A, adds C
sorted(ie.hardEvidenceNodes())
# [0, 2]
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
