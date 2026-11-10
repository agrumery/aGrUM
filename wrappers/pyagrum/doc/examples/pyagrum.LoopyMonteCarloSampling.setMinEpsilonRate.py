import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.setMinEpsilonRate(1e-3)
ie.minEpsilonRate()
# 0.001
