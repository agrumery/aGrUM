import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.exists(bn.idFromName("C"))
# True
fragment.exists(bn.idFromName("E"))  # E is in bn but not in the fragment
# False
