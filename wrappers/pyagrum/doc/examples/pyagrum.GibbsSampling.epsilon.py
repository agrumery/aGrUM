import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
round(ie.epsilon(), 4)  # the default value, given by the configuration
# 0.2019
ie.setEpsilon(1e-3)
ie.epsilon()
# 0.001
