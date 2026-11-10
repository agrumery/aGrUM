import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.minimalCondSet(bn.idFromName("C"), bn.ids(["A", "B", "D"]))  # A is not needed
# {1, 3}
