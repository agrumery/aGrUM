import pyagrum as gum

g = gum.fastChainGraph("A->B-C<-D")
sorted(g.arcs())
# [(0, 1), (3, 2)]
sorted(g.edges())
# [(1, 2)]
