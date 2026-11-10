import pyagrum as gum

g = gum.MarkedGraph()
g.addNodes(2)
g.emptyEdges()
# True
g.addEdge(0, 1)
g.emptyEdges()
# False
