import pyagrum as gum

bn = gum.fastBN("X->Y")
cm = gum.CausalModel(bn)
cm.assumeSpurious("X", "Y")
cm.isAssumedSpurious("X", "Y")
# True
cm.existsArc("X", "Y")  # no longer a causal arc
# False
