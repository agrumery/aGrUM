import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setBurnIn(200)
dist.setMaxIter(1000)
dist.compute()  # a dict of divergences
dist.messageApproximationScheme()
# 'stopped with max iteration=1000'
