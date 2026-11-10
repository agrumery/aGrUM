import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.periodSize()
# 100
ie.setPeriodSize(50)
ie.periodSize()
# 50
