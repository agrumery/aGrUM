import pyagrum as gum

p = gum.fastBN("A->B")
q = gum.fastBN("A->B")
dist = gum.GibbsBNdistance(p, q)
dist.setBurnIn(0)
dist.setPeriodSize(10)
dist.setEpsilon(0.1)
dist.initApproximationScheme()
dist.updateApproximationScheme(10)
dist.continueApproximationScheme(0.5)  # a first error
# True
dist.updateApproximationScheme(10)
dist.continueApproximationScheme(0.05)  # an error below epsilon: the scheme stops
# False
dist.messageApproximationScheme()
# 'stopped with epsilon=0.1'
