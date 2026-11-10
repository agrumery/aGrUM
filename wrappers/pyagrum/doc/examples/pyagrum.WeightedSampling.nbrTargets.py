import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.nbrTargets()
# 3
ie.setTargets({"A"})
ie.nbrTargets()
# 1
