import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.eraseAllTargets()
ie.addTarget("B")
ie.targets()
# {1}
