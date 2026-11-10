import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.setNumberOfThreads(2)
ie.getNumberOfThreads()
# 2
