import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.addJointTarget({"A", "B"})
ie.isJointTarget({"A", "B"})
# True
ie.isJointTarget({"B", "C"})
# False
