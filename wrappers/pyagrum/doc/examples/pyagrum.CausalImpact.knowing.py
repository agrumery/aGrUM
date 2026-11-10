import pyagrum as gum

bn = gum.fastBN("W->X->Y;W->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X", knowing="W")
ci.knowing()
# {0}
