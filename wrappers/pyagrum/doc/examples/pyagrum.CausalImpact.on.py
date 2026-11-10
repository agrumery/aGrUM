import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Z", doing="X")
ci.on()
# {2}
