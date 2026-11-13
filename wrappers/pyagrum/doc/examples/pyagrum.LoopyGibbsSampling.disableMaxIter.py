import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.isEnabledMaxIter()
# True
ie.disableMaxIter()
ie.isEnabledMaxIter()
# False
