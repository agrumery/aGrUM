import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
sorted(ie.BN().names())
# ['A', 'B', 'C']
ie.BN().sizeArcs()
# 2
