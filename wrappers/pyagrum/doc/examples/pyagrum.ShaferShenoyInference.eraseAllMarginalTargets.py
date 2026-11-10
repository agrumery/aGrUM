import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addTarget("A")
ie.addJointTarget({"B", "C"})
ie.eraseAllMarginalTargets()
ie.nbrTargets()
# 0
ie.nbrJointTargets()  # the joint targets are kept
# 1
