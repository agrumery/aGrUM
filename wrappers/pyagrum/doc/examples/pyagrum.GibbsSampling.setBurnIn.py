import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.burnIn()
# 300
ie.setBurnIn(1000)
ie.burnIn()
# 1000
