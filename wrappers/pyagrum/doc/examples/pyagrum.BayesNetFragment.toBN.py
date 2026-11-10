import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
bn2 = fragment.toBN()  # a new and independent BayesNet
type(bn2).__name__
# 'BayesNet'
sorted(bn2.names())
# ['A', 'B', 'C', 'D']
