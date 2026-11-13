import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.WeightedSampling(bn)
ie.isEnabledMaxIter()  # is the number of iterations a stopping criterion
# True
