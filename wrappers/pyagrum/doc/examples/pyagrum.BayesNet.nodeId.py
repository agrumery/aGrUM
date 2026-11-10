import pyagrum as gum

bn = gum.fastBN("A->B->C")
v = bn.variableFromName("B")
bn.nodeId(v)
# 1
