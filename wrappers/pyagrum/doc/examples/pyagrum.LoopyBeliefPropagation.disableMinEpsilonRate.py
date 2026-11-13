import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.isEnabledMinEpsilonRate()
# True
ie.disableMinEpsilonRate()
ie.isEnabledMinEpsilonRate()
# False
