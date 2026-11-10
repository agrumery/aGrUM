import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setEpsilon(1e-3)  # stop as soon as the error of a period is below 1e-3
dist.epsilon()
# 0.001
