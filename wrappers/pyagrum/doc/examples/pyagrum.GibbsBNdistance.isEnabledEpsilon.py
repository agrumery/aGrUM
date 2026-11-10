import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.isEnabledEpsilon()  # a stopping criterion enabled by default
# True
