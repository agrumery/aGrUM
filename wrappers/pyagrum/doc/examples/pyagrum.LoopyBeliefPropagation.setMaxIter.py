import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyBeliefPropagation(bn)
ie.maxIter()
# 100
ie.setMaxIter(5000)
ie.maxIter()
# 5000
