import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.isEnabledMaxIter()  # is the number of iterations a stopping criterion
# True
