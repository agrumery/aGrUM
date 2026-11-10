import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.setTargets({"A", "C"})
sorted(ie.targets())
# [0, 2]
