import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setBurnIn(100)
dist.initApproximationScheme()
dist.remainingBurnIn()
# 100
dist.updateApproximationScheme(30)
dist.remainingBurnIn()
# 70
