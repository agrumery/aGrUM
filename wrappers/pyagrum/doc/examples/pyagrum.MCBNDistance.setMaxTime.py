import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setMaxTime(2.5)  # in seconds
dist.maxTime()
# 2.5
