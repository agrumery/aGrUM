import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.5, 0.5])
bn.cpt("B").fillWith([0.75, 0.25, 0.5, 0.5])
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("B")
inst = fragment.completeInstantiation()
inst.chgVal("B", 1)
fragment.log2JointProbability(inst)  # log2(0.5 * 0.25)
# -3.0
