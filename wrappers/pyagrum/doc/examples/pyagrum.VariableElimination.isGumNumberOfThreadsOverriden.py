import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.isGumNumberOfThreadsOverriden()
# False
ie.setNumberOfThreads(2)
ie.isGumNumberOfThreadsOverriden()
# True
