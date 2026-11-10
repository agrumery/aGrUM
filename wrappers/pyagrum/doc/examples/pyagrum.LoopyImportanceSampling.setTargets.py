import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.setTargets({"A", "C"})
sorted(ie.targets())
# [0, 2]
