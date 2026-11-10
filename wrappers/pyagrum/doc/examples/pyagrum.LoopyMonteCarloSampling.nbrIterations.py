import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.setEpsilon(1e-10)
ie.setMaxIter(1000)
ie.makeInference()
ie.nbrIterations()
# 1000
