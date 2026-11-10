import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([1.0, 0.0, 0.3, 0.7])
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("B")  # the CPT of C is not in the fragment
fragment.minParam()
# 0.0
