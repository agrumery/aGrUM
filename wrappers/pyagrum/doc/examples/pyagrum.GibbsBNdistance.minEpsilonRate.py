import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.minEpsilonRate()
# 1e-10
dist.setMinEpsilonRate(1e-4)
dist.minEpsilonRate()
# 0.0001
