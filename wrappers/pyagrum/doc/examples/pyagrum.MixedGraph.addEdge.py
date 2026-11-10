import pyagrum as gum

g = gum.MixedGraph()
g.addNodes(3)
# {0, 1, 2}
g.addEdge(1, 2)
g.addArc(0, 1)
g.edges(), g.arcs()
# ({(1, 2)}, {(0, 1)})
