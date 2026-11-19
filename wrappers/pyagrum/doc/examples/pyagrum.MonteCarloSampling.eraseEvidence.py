import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.MonteCarloSampling(bn)
ie.setEvidence({"A": 1, "C": 0})
ie.eraseEvidence("A")
ie.hardEvidenceNodes()
# {2}
