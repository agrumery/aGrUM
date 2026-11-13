import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.setEvidence({"A": 1})
ie.setEpsilon(1e-10)
ie.setMaxIter(5000)
ie.makeInference()
ie.messageApproximationScheme()
# 'stopped with max iteration=5000'
exact = gum.LazyPropagation(bn)
exact.setEvidence({"A": 1})
exact.makeInference()
abs(ie.posterior("B")[1] - exact.posterior("B")[1]) < 0.05  # an approximation of the exact posterior
# True
