import pyagrum as gum

bn = gum.fastBN("A->B;C->D")
cm = gum.CausalModel(bn)
sorted(sorted(cm.nameFromId(i) for i in cc) for cc in cm.connectedComponentsList().values())
# [['A', 'B'], ['C', 'D']]
