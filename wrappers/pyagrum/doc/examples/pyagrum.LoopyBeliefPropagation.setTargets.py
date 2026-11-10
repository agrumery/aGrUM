import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.setTargets({"A", "C"})
sorted(ie.targets())
# [0, 2]
