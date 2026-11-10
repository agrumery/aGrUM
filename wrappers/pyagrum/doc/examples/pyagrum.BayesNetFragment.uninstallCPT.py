import pyagrum as gum

bn = gum.fastBN("A->B")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.installMarginal("B", gum.Tensor().add(bn.variable("B")).fillWith([0.3, 0.7]))
fragment.checkConsistency()
# True
fragment.uninstallCPT("B")  # back to the CPT of bn: P(B|A), but A is not installed
fragment.checkConsistency()
# False
