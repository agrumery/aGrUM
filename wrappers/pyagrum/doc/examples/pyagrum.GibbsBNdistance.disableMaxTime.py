import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.isEnabledMaxTime()
# True
dist.disableMaxTime()
dist.isEnabledMaxTime()
# False
