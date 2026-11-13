import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.isEnabledMaxTime()  # is the time a stopping criterion
# True
