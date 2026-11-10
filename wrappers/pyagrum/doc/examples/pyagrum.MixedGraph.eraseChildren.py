import pyagrum as gum

g = gum.fastMixedGraph("A->B;A->C;A-D")
g.eraseChildren(0)  # the edge A-D is kept
g.arcs(), g.edges()
# (set(), {(0, 3)})
