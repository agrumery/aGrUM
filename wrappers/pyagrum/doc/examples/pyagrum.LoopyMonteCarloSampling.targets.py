import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
sorted(ie.targets())  # all nodes by default
# [0, 1, 2]
