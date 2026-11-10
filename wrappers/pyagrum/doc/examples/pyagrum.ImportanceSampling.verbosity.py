import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.verbosity()
# False
ie.setVerbosity(True)
ie.verbosity()
# True
