import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.periodSize()
# 100
ie.setPeriodSize(50)
ie.periodSize()
# 50
