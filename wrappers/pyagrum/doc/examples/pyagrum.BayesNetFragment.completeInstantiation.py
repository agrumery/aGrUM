import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
inst = fragment.completeInstantiation()
inst.nbrDim()  # only the installed variables
# 4
