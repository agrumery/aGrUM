import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Z", doing="X")
sorted(ci.cm().names())
# ['X', 'Y', 'Z']
