import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
round(ie.jointMutualInformation(["A", "B"]), 4)  # the mutual information I(A;B)
# 0.1826
round(ie.jointMutualInformation(["A", "B", "C"]), 4)
# -0.0456
