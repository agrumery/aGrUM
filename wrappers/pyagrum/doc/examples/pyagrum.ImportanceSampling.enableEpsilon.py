import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.disableEpsilon()
ie.enableEpsilon()
ie.isEnabledEpsilon()
# True
