import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
bn.eraseArc("D", "C")  # the fragment is notified
sorted(fragment.arcs())
# [(0, 1), (1, 2)]
