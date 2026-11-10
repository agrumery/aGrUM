import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.isDrawnAtRandom()
# True
ie.setDrawnAtRandom(False)
ie.isDrawnAtRandom()
# False
