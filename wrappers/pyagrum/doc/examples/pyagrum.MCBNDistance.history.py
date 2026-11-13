import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setVerbosity(True)  # the history is kept only in verbose mode
dist.disableEpsilon()  # only maxIter stops the scheme
dist.disableMinEpsilonRate()
dist.setMaxIter(1000)
dist.compute()  # a dict of divergences
len(dist.history())  # one error per period
# 5
