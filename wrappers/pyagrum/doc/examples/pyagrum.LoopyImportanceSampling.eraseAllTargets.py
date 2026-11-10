import pyagrum as gum

bn = gum.fastBN("A->B<-C")
ie = gum.LoopyImportanceSampling(bn)
ie.setTargets({"A", "B"})
ie.nbrTargets()
# 2
ie.eraseAllTargets()
ie.nbrTargets()
# 0
