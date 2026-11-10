import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;B-D")
g.eraseNeighbours(1)  # the arc A->B is kept
g.arcs(), g.edges()
# ({(0, 1)}, set())
