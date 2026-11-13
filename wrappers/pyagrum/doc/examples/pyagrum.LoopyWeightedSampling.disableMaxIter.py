import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.isEnabledMaxIter()
# True
ie.disableMaxIter()
ie.isEnabledMaxIter()
# False
