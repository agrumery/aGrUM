import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.addTarget("A")
ie.isTarget("A"), ie.isTarget("B")
# (True, False)
