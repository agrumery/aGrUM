import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.isEnabledMaxTime()
# True
ie.disableMaxTime()
ie.isEnabledMaxTime()
# False
