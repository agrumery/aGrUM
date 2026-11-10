import pyagrum as gum

bn = gum.fastBN("X->Y->Z")
cm = gum.CausalModel(bn)
cm.assumeSpurious("X", "Y")
cm.isAssumedSpurious("X", "Y")
# True
cm.isAssumedSpurious("Y", "Z")
# False
