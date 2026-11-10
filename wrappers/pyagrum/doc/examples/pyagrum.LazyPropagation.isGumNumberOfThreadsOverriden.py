import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.isGumNumberOfThreadsOverriden()
# False
ie.setNumberOfThreads(2)
ie.isGumNumberOfThreadsOverriden()
# True
