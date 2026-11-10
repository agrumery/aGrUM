import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.setNumberOfThreads(2)
ie.getNumberOfThreads()
# 2
