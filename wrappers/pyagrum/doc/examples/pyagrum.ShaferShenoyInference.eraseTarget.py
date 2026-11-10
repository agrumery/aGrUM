import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addTarget("A")
ie.addTarget("C")
ie.eraseTarget("A")
sorted(ie.targets())
# [2]
