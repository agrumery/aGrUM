import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.minEpsilonRate()
# 1e-05
ie.setMinEpsilonRate(1e-4)
ie.minEpsilonRate()
# 0.0001
