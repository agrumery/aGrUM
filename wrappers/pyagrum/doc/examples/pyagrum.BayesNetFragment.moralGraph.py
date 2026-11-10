import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
sorted(fragment.moralGraph().edges())  # the parents B and D of C are married
# [(0, 1), (1, 2), (1, 3), (2, 3)]
