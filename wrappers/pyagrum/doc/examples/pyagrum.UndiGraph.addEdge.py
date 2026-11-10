import pyagrum as gum

g = gum.UndiGraph()
g.addNodes(2)
# {0, 1}
g.addEdge(1, 0)
g.edges()
# {(0, 1)}
