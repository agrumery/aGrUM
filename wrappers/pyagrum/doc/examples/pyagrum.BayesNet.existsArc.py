import pyagrum as gum

bn = gum.fastBN("A->B")
bn.existsArc("A", "B")
# True
bn.existsArc("B", "A")
# False
