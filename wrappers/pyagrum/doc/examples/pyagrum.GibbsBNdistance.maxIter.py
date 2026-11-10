import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.maxIter()
# 10000000
dist.setMaxIter(1000)
dist.maxIter()
# 1000
