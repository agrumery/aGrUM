import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.eraseEdge(1, 2)
g.arcs(), g.edges()
# ({(0, 1)}, set())
