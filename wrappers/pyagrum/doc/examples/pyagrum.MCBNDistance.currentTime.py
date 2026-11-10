import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setMaxIter(1000)
dist.compute()  # a dict of divergences
dist.currentTime() > 0  # duration of the computation, in seconds
# True
