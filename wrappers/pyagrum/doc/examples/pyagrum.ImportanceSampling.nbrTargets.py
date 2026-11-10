import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.ImportanceSampling(bn)
ie.nbrTargets()  # all nodes by default
# 3
ie.setTargets({"A"})
ie.nbrTargets()
# 1
