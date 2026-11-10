import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.nbrTargets()
# 3
ie.setTargets({"A"})
ie.nbrTargets()
# 1
