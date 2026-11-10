import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyWeightedSampling(bn)
ie.setEpsilon(0.05)
ie.epsilon()
# 0.05
