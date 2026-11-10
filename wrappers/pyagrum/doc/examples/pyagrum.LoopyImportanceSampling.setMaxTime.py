import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.maxTime()
# 6000.0
ie.setMaxTime(10.0)
ie.maxTime()
# 10.0
