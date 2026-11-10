import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D")
g.hasMixedOrientedPath(0, 3)
# True
g.hasMixedOrientedPath(3, 0)
# False
