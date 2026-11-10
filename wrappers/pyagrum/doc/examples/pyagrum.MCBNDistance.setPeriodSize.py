import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.MCBNDistance(p, q)
dist.setPeriodSize(500)  # check the stopping criteria every 500 iterations
dist.periodSize()
# 500
