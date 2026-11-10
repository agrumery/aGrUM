import pyagrum as gum

bn = gum.fastBN("A->C;B->C")
cm = gum.CausalModel(bn)
sorted(cm.nameFromId(i) for i in cm.parents("C"))
# ['A', 'B']
