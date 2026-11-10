import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.minEpsilonRate()
# 0.006737946999085467
ie.setMinEpsilonRate(1e-4)
ie.minEpsilonRate()
# 0.0001
