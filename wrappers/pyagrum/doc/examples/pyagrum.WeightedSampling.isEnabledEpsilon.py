import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.isEnabledEpsilon()  # is epsilon a stopping criterion
# True
