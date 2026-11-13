import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.isEnabledMaxIter()
# True
ie.disableMaxIter()
ie.isEnabledMaxIter()
# False
