import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.periodSize()
# 100
ie.setPeriodSize(50)
ie.periodSize()
# 50
