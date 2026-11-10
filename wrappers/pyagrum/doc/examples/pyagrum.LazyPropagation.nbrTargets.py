import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.addTarget("A")
ie.addTarget("C")
ie.nbrTargets()
# 2
