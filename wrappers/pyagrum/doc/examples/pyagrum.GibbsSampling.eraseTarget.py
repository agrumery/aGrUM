import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.setTargets({"A", "B"})
ie.eraseTarget("A")
sorted(ie.targets())
# [1]
