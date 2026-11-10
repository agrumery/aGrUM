import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyMonteCarloSampling(bn)
# internal sampling loop: use makeInference() instead
ie.makeInference_()
round(ie.posterior("B").sum(), 4)
# 1.0
