import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.isEnabledEpsilon()
# True
ie.disableEpsilon()
ie.isEnabledEpsilon()
# False
