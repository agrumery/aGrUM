import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.disableMinEpsilonRate()
ie.enableMinEpsilonRate()
ie.isEnabledMinEpsilonRate()
# True
