import pyagrum as gum

g = gum.DiGraph()
g.addNodes(4)
# {0, 1, 2, 3}
g.addArc(0, 2)
g.addArc(1, 2)
g.addArc(2, 3)
g.parents(2)
# {0, 1}
g.parents(0)
# set()
