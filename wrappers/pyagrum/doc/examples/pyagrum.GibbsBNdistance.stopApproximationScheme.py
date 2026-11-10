import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.initApproximationScheme()
dist.stopApproximationScheme()
dist.messageApproximationScheme()
# 'stopped on request'
