import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D")
sorted(g.arcs())
# [(0, 1), (2, 3)]
