import pyagrum as gum

bn = gum.fastBN("A->C<-B;C->D;E->D")
mag = bn.moralizedAncestralGraph(["C"])  # only the ancestors of C are kept
sorted(mag.nodes())
# [0, 1, 2]
sorted(mag.edges())
# [(0, 1), (0, 2), (1, 2)]
