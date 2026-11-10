import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.hasSameStructure(gum.fastBN("A->B->C<-D"))
# True
fragment.hasSameStructure(bn)
# False
