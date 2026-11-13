import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.disableMinEpsilonRate()
ie.enableMinEpsilonRate()
ie.isEnabledMinEpsilonRate()
# True
