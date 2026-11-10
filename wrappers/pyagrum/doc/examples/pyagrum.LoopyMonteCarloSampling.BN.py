import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
sorted(ie.BN().names())
# ['A', 'B', 'C']
ie.BN().sizeArcs()
# 2
