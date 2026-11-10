import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.eraseAllTargets()
ie.nbrTargets()
# 0
ie.addTarget("B")
ie.targets()
# {1}
