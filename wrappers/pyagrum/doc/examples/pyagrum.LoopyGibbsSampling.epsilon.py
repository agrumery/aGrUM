import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyGibbsSampling(bn)
ie.epsilon()
# 0.20189651799465538
ie.setEpsilon(1e-3)
ie.epsilon()
# 0.001
