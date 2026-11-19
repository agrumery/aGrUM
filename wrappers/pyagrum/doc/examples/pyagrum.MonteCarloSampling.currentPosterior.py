import pyagrum as gum

bn = gum.fastBN("A->B<-C", uniform=True)
ie = gum.MonteCarloSampling(bn)
ie.addEvidence("A", 1)
ie.makeInference()
round(ie.currentPosterior("B").sum(), 4)
# 1.0
