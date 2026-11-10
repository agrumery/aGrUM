import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setMinEpsilonRate(1e-4)
dist.minEpsilonRate()
# 0.0001
