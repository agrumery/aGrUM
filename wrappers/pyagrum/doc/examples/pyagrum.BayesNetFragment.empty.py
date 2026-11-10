import pyagrum as gum

bn = gum.fastBN("A->B")
fragment = gum.BayesNetFragment(bn)
fragment.empty()
# True
fragment.installNode("A")
fragment.empty()
# False
