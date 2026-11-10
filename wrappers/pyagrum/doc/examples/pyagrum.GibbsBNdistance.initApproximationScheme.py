import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.initApproximationScheme()  # what compute() does first
dist.messageApproximationScheme()
# 'in progress'
dist.nbrIterations()
# 0
