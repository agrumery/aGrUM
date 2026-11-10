import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.BN().size()
# 3
sorted(ie.BN().names())
# ['A', 'B', 'C']
