import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.clear()
bn.size()
# 0
bn.empty()
# True
