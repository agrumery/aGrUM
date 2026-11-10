import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.addTarget("B")
ie.addTarget("C")
sorted(ie.targets())
# [1, 2]
