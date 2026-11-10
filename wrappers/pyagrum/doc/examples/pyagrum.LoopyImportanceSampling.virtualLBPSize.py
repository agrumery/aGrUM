import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.virtualLBPSize()
# 5000.0
ie.setVirtualLBPSize(1000.0)
ie.virtualLBPSize()
# 1000.0
