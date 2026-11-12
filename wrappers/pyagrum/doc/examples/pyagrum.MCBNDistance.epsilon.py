import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.epsilon()
# 1e-05
dist.setEpsilon(1e-3)
dist.epsilon()
# 0.001
