import pyagrum as gum

bn = gum.fastBN("A[3]->B[4]->C[5]")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("B")
fragment.maxVarDomainSize()
# 4
