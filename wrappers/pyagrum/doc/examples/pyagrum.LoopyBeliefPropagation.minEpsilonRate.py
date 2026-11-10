import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.minEpsilonRate()
# 1e-10
ie.setMinEpsilonRate(1e-4)
ie.minEpsilonRate()
# 0.0001
