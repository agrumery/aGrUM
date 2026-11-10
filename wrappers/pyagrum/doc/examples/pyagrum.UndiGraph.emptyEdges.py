import pyagrum as gum

g = gum.UndiGraph()
g.addNodes(2)
# {0, 1}
g.emptyEdges()
# True
g.addEdge(0, 1)
g.emptyEdges()
# False
