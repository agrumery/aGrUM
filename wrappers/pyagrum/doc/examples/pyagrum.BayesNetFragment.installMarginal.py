import pyagrum as gum

bn = gum.fastBN("A->B->C")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.checkConsistency()  # A is not installed
# False
fragment.installMarginal("B", gum.Tensor().add(bn.variable("B")).fillWith([0.3, 0.7]))
fragment.checkConsistency()
# True
fragment.cpt("B").tolist()
# [0.3, 0.7]
