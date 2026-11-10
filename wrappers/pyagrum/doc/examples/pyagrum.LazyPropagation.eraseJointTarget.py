import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.addJointTarget({"A", "B"})
ie.addJointTarget({"B", "C"})
ie.eraseJointTarget({"A", "B"})
ie.jointTargets()
# [{1, 2}]
