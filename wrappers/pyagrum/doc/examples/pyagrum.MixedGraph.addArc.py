import pyagrum as gum

g = gum.MixedGraph()
g.addNodes(3)
# {0, 1, 2}
g.addArc(0, 1)
g.addEdge(1, 2)
g.arcs(), g.edges()
# ({(0, 1)}, {(1, 2)})
