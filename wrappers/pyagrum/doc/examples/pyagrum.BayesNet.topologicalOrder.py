import pyagrum as gum

bn = gum.fastBN("C->A;A->B;C->B")
bn.topologicalOrder()
# [0, 1, 2]
