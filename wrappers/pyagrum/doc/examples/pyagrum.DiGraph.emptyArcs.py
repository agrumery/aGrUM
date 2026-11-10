import pyagrum as gum

g = gum.DiGraph()
g.addNodes(2)
# {0, 1}
g.emptyArcs()
# True
g.addArc(0, 1)
g.emptyArcs()
# False
