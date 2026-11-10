import pyagrum as gum

bn = gum.fastBN("A->B")
bn.exists(1)
# True
bn.exists(5)
# False
