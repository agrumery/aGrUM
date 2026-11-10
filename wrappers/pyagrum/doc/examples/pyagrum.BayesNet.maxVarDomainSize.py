import pyagrum as gum

bn = gum.fastBN("A[3]->B[5]->C")
bn.maxVarDomainSize()
# 5
