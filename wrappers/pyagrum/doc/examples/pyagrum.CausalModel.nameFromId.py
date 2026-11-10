import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn)
cm.nameFromId(1)
# 'B'
