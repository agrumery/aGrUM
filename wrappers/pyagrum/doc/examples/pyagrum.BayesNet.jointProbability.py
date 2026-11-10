import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
inst = bn.completeInstantiation()
inst.chgVal("A", 1)
inst.chgVal("B", 0)
round(bn.jointProbability(inst), 4)  # P(A=1) * P(B=0 | A=1)
# 0.24
