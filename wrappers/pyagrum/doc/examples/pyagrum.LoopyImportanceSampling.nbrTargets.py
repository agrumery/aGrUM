import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.nbrTargets()  # all nodes by default
# 3
ie.setTargets({"A"})
ie.nbrTargets()
# 1
