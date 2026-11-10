import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addTarget("B")
ie.setTargets({"A", "C"})
sorted(ie.targets())
# [0, 2]
