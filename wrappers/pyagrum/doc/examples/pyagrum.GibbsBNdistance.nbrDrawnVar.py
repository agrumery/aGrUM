import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.nbrDrawnVar()  # variables drawn at each iteration of the Gibbs sampler
# 1
