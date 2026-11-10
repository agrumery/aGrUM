import pyagrum as gum

bn = gum.fastBN("A[10]->B[10]->C[10]")
bn.log10DomainSize()  # log10(10 * 10 * 10)
# 3.0
