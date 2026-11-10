import pyagrum as gum

bn = gum.fastBN("A->C;B")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")
fragment.installNode("B")
bn.addArc("B", "C")  # the fragment is notified, but no arc is added to it
fragment.existsArc("B", "C")
# False
fragment.checkConsistency("C")  # the CPT of C in bn now depends on B
# False
