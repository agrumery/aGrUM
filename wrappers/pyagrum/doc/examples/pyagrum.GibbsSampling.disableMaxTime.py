import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.isEnabledMaxTime()
# True
ie.disableMaxTime()
ie.isEnabledMaxTime()
# False
