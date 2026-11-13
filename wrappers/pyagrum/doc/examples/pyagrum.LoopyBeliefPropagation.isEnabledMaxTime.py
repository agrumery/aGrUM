import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.isEnabledMaxTime()  # is the time a stopping criterion
# False
