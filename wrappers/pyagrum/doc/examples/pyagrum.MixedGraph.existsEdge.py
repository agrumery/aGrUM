import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.existsEdge(2, 1)
# True
g.existsEdge(0, 1)  # A->B is an arc, not an edge
# False
