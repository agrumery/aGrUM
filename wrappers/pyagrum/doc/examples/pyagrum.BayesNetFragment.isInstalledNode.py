import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.isInstalledNode("C")
# True
fragment.isInstalledNode(bn.idFromName("E"))
# False
