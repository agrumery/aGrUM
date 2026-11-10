import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setDrawnAtRandom(False)  # draw the variables in a fixed order
dist.isDrawnAtRandom()
# False
