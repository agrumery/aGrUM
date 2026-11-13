import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.isEnabledMinEpsilonRate()  # is the minimal epsilon rate a stopping criterion
# True
