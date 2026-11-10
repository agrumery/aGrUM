import pyagrum as gum

bn = gum.fastBN("X->Z->Y")
bn.cpt("X").fillWith([0.5, 0.5])
bn.cpt("Z").fillWith([0.9, 0.1, 0.2, 0.8])
bn.cpt("Y").fillWith([0.7, 0.3, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
it = gum.InformationTheory(ie, "X", "Y", "Z")
round(it.entropyXYgivenZ(), 4)  # H(X,Y|Z)
# 1.2984
