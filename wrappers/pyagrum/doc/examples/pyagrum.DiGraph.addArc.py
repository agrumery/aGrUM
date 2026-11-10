import pyagrum as gum

g = gum.DiGraph()
a, b = g.addNode(), g.addNode()
g.addArc(a, b)
g.arcs()
# {(0, 1)}
