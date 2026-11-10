import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->A")
sorted(g.arcs())
# [(0, 1), (2, 0)]
sorted(g.edges())
# [(1, 2)]
