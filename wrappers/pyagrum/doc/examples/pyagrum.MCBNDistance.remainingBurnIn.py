import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.initApproximationScheme()
dist.remainingBurnIn()  # no burn-in: the samples are independent
# 0
