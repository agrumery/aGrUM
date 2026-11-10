import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.messageApproximationScheme()
# 'undefined state'
ie.setEpsilon(1e-10)
ie.setMaxIter(1000)
ie.makeInference()
ie.messageApproximationScheme()
# 'stopped with max iteration=1000'
