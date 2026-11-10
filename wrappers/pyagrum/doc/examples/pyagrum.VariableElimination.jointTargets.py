import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.VariableElimination(bn)
ie.addJointTarget({"A", "B"})
ie.addJointTarget({"B", "C"})
ie.jointTargets()
# [{0, 1}, {1, 2}]
