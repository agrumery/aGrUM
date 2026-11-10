import pyagrum as gum

g = gum.fastMixedGraph("A-B;C")
g.emptyArcs()  # an edge is not an arc
# True
g.addArc(1, 2)
g.emptyArcs()
# False
