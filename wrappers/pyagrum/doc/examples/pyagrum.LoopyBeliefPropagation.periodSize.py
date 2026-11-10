import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.periodSize()
# 1
ie.setPeriodSize(50)
ie.periodSize()
# 50
