import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addTarget("B")
sorted(ie.targets())
# [1]
ie.isTarget("A")
# False
