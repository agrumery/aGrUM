import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.eraseTarget("A")
sorted(ie.targets())
# [1, 2]
