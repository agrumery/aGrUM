import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.disableEpsilon()
dist.enableEpsilon()
dist.isEnabledEpsilon()
# True
