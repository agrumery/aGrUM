import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.setNumberOfThreads(2)
ie.getNumberOfThreads()
# 2
ie.isGumNumberOfThreadsOverriden()
# True
ie.setNumberOfThreads(0)  # back to aGrUM's default number of threads
ie.isGumNumberOfThreadsOverriden()
# False
