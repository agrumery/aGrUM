import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.addJointTarget({"A", "B"})
ie.addJointTarget({"B", "C"})
ie.nbrJointTargets()
# 2
ie.eraseAllJointTargets()
ie.nbrJointTargets()
# 0
