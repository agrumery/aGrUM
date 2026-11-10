import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addTarget("A")
ie.isTarget("A")
# True
ie.isTarget("B")
# False
