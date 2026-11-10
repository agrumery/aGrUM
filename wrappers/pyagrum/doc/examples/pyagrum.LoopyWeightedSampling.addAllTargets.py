import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.eraseAllTargets()
ie.addTarget("A")
ie.addAllTargets()
sorted(ie.targets())
# [0, 1, 2]
