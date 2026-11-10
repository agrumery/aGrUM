import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.existsArc(0, 1)
# True
g.existsArc(1, 2)  # B-C is an edge, not an arc
# False
