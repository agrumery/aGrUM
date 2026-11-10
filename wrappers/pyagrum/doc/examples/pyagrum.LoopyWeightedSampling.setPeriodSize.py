import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.setPeriodSize(50)
ie.periodSize()
# 50
