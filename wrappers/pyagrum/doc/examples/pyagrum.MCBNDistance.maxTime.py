import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.maxTime()  # in seconds
# 6000.0
dist.setMaxTime(2.5)
dist.maxTime()
# 2.5
