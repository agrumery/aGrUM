import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.disableMaxIter()
ie.enableMaxIter()
ie.isEnabledMaxIter()
# True
