import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([1.0, 0.0, 0.3, 0.7])
bn.maxParam()
# 1.0
