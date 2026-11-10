import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.eraseArc(0, 1)
g.arcs(), g.edges()
# (set(), {(1, 2)})
