import pyagrum as gum

bn = gum.fastBN("A[10]->B[10]->C[10]")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("B")
fragment.log10DomainSize()  # log10(10 * 10)
# 2.0
