import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.isEnabledMaxIter()
# True
ie.disableMaxIter()
ie.isEnabledMaxIter()
# False
