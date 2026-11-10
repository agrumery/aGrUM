import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.addTarget("A")
ie.isTarget("A"), ie.isTarget("B")
# (True, False)
