import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")
sorted(fragment.names())
# ['A', 'B', 'C', 'D']
fragment.checkConsistency()
# True
