import pyagrum as gum

bn = gum.fastBN("A->B->C<-D;C->E")
fragment = gum.BayesNetFragment(bn)
fragment.installAscendants("C")  # A, B, C, D
mag = fragment.moralizedAncestralGraph(["B", "D"])
sorted(mag.nodes())
# [0, 1, 3]
mag.edges()
# {(0, 1)}
