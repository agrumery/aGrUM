import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.setTargets({"A", "B"})
ie.eraseTarget("A")
sorted(ie.targets())
# [1]
