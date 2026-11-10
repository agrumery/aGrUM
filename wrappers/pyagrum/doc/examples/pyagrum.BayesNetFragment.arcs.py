import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
sorted(fragment.arcs())  # no arc C->E: E is not installed
# [(0, 1), (1, 2), (3, 2)]
