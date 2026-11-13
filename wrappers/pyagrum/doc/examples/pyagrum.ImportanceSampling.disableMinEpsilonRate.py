import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.isEnabledMinEpsilonRate()
# True
ie.disableMinEpsilonRate()
ie.isEnabledMinEpsilonRate()
# False
