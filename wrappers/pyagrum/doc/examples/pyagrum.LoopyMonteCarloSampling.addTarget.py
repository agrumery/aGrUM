import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
ie.eraseAllTargets()
ie.addTarget("B")
ie.targets()
# {1}
