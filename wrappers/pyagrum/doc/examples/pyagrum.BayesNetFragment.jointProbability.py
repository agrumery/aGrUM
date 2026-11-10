import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("B")
inst = fragment.completeInstantiation()
inst.chgVal("A", 1)
round(fragment.jointProbability(inst), 4)  # P(A=1, B=0)
# 0.24
