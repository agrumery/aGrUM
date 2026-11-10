import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.setVirtualLBPSize(1000)
ie.virtualLBPSize()
# 1000.0
