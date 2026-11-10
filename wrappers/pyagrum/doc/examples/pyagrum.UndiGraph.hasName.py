import pyagrum as gum

g = gum.UndiGraph()
g.addNodes(2)
# {0, 1}
g.setName(0, "A")
g.hasName(0)
# True
g.hasName(1)
# False
