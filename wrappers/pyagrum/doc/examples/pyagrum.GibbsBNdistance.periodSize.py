import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.periodSize()  # iterations between two checks of the stopping criteria
# 200
dist.setPeriodSize(500)
dist.periodSize()
# 500
