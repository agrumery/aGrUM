import pyagrum as gum

bn = gum.fastBN("A->B")
fragment = gum.BayesNetFragment(bn)
fragment.installNode("B")
fragment.installMarginal("B", gum.Tensor().add(bn.variable("B")).fillWith([0.4, 0.6]))
fragment.cpt("B").tolist()  # the local CPT installed in the fragment
# [0.4, 0.6]
bn.cpt("B").names  # the referenced BN is unchanged
# ('B', 'A')
