import pyagrum as gum

bn = gum.fastBN("A->B->C")
cm = gum.CausalModel(bn)
cm.existsArc("A", "B")
# True
cm.existsArc("B", "A")
# False
