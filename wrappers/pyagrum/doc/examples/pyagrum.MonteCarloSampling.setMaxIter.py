import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.setMaxIter(500)
ie.setEpsilon(1e-10)
ie.makeInference()
ie.maxIter(), ie.nbrIterations()
# (500, 500)
