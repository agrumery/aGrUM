import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setBurnIn(0)
dist.setPeriodSize(10)
dist.initApproximationScheme()
dist.updateApproximationScheme(10)
dist.startOfPeriod()
# True
dist.updateApproximationScheme(3)
dist.startOfPeriod()
# False
