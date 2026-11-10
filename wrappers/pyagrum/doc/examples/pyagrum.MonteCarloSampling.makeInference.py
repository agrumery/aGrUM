import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.setEvidence({"A": 1})
ie.setEpsilon(1e-10)
ie.setMaxIter(1000)
ie.makeInference()
ie.messageApproximationScheme()
# 'stopped with max iteration=1000'
ie.posterior("A").tolist()
# [0.0, 1.0]
