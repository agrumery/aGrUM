import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.burnIn()
# 300
ie.setBurnIn(1000)
ie.burnIn()
# 1000
