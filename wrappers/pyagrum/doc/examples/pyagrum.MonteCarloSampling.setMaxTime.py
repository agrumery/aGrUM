import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.MonteCarloSampling(bn)
ie.setMaxTime(0.5)  # in seconds
ie.maxTime()
# 0.5
