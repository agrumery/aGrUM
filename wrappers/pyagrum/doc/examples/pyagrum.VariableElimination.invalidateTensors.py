import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
ie = gum.VariableElimination(bn)
ie.makeInference()
[round(p, 4) for p in ie.posterior("B").tolist()]
# [0.42, 0.58]
bn.cpt("A").fillWith([0.7, 0.3])  # changed in place: the inference is not notified
ie.invalidateTensors()
ie.makeInference()
[round(p, 4) for p in ie.posterior("B").tolist()]  # 0.7 * 0.9 + 0.3 * 0.3
# [0.72, 0.28]
