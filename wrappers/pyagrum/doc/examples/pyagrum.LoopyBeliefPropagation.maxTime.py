import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.maxTime()
# 1.0
ie.setMaxTime(10.0)
ie.maxTime()
# 10.0
