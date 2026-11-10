import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.epsilon()
# 1e-08
ie.setEpsilon(1e-3)
ie.epsilon()
# 0.001
