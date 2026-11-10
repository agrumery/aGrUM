import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
fragment.uninstallNode("A")
sorted(fragment.names())
# ['B', 'C', 'D']
fragment.checkConsistency()  # B has lost its parent A
# False
