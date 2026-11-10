import pyagrum as gum

bn = gum.fastBN("A->B;A->C")
cm = gum.CausalModel(bn)
sorted(cm.nameFromId(i) for i in cm.children("A"))
# ['B', 'C']
