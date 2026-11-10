import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.isEnabledMinEpsilonRate()
# True
dist.disableMinEpsilonRate()
dist.isEnabledMinEpsilonRate()
# False
