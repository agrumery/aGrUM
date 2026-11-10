import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.epsilon()
# 0.01
ie.setEpsilon(1e-3)
ie.epsilon()
# 0.001
