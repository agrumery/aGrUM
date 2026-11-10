import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.isTarget("A")
# True
ie.eraseTarget("A")
ie.isTarget("A")
# False
