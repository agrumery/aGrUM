import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.verbosity()
# False
ie.setVerbosity(True)
ie.verbosity()
# True
