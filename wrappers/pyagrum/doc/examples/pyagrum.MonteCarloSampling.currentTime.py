import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.setMaxIter(1000)
ie.makeInference()
ie.currentTime() >= 0  # in seconds
# True
