import pyagrum as gum

bn = gum.fastBN("X->Y")
cm = gum.CausalModel(bn)
cm.assumeSpurious("X", "Y")
cm.assumeNonSpurious("X", "Y")
cm.isAssumedSpurious("X", "Y")
# False
