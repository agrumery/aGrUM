import pyagrum as gum

g = gum.MarkedGraph()
g.addNodeWithId(5)
g.nodes()
# {5}
g.existsNode(5), g.existsNode(0)
# (True, False)
