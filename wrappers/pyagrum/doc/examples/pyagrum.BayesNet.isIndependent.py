import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.isIndependent("A", "C", [])
# False
bn.isIndependent("A", "C", ["B"])
# True
