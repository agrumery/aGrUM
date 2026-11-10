import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
print(fragment.adjacencyMatrix())
# [[0 1 0 0]
#  [0 0 1 0]
#  [0 0 0 0]
#  [0 0 1 0]]
