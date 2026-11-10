import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.addTarget("A")
ie.addTarget("C")
ie.nbrTargets()
# 2
