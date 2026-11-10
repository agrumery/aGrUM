import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
# with no target declared, every node is a target
sorted(ie.targets())
# [0, 1, 2]
ie.addTarget("C")
sorted(ie.targets())
# [2]
