import pyagrum as gum

bn = gum.fastBN("A->C<-B")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")
pot = gum.Tensor().add(bn.variable("C")).add(bn.variable("B")).fillWith([0.1, 0.9, 0.8, 0.2])  # P(C|B)
fragment.installCPT("C", pot)
fragment.parents("C")  # A is no longer a parent of C in the fragment
# {2}
fragment.checkConsistency()
# True
