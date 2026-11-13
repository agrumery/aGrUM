import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.disableMaxIter()
ie.enableMaxIter()
ie.isEnabledMaxIter()
# True
