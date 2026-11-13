import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.disableEpsilon()  # only maxIter stops the scheme
dist.disableMinEpsilonRate()
dist.setMaxIter(1000)
dist.compute()  # a dict of divergences
dist.nbrIterations()  # checked at the end of each period
# 1000
