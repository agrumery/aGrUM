import pyagrum as gum

bn = gum.fastBN("A->B->C;A->C")
sorted(bn.arcs())
# [(0, 1), (0, 2), (1, 2)]
