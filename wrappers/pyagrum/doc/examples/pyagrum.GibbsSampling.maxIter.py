import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.GibbsSampling(bn)
ie.maxIter()
# 10000000
ie.setMaxIter(5000)
ie.maxIter()
# 5000
