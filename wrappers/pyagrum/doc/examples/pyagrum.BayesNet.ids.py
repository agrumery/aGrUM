import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.ids(["C", "A"])
# [2, 0]
