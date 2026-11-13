import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.isEnabledEpsilon()  # is epsilon a stopping criterion
# True
