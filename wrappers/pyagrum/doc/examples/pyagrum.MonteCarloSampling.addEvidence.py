import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.MonteCarloSampling(bn)
ie.addEvidence("A", 1)  # by index
ie.addEvidence("C", "0")  # by label
sorted(ie.hardEvidenceNodes())
# [0, 2]
ie.makeInference()
ie.posterior("A").tolist()
# [0.0, 1.0]
