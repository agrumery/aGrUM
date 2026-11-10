import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.isIndependent("A", "D", [])
# True
fragment.isIndependent("A", "D", ["C"])
# False
