import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.disableEpsilon()
ie.enableEpsilon()
ie.isEnabledEpsilon()
# True
