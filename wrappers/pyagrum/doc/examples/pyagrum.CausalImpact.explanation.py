import pyagrum as gum

bn = gum.fastBN("Z->X->Y;Z->Y")
cm = gum.CausalModel(bn)
ci = gum.CausalImpact(cm, on="Y", doing="X")
ci.explanation()
# "backdoor ['Z'] found."
