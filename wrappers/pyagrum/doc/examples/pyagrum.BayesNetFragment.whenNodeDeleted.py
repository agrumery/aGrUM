import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
bn.erase("D")  # the fragment is notified and uninstalls D
sorted(fragment.names())
# ['A', 'B', 'C']
