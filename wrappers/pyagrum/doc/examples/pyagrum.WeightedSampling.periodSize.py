import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.periodSize()  # samples between two checks of the stopping criteria
# 100
