import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.periodSize()  # samples between two checks of the stopping criteria
# 100
