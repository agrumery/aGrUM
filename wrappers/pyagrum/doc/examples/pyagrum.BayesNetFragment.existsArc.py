import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.existsArc("B", "C")
# True
fragment.existsArc(bn.idFromName("C"), bn.idFromName("E"))  # E is not installed
# False
