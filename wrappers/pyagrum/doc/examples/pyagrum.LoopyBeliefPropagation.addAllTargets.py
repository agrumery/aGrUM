import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.addTarget("A")
sorted(ie.targets())
# [0]
ie.addAllTargets()
sorted(ie.targets())
# [0, 1, 2]
