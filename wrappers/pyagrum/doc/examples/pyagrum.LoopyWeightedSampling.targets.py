import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
sorted(ie.targets())  # all nodes by default
# [0, 1, 2]
