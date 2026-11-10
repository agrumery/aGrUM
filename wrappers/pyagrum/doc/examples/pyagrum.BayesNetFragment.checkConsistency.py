import pyagrum as gum

bn = gum.fastBN("A->B->C")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.checkConsistency("B")  # the parent A of B is not installed
# False
fragment.installNode("A")
fragment.checkConsistency()
# True
