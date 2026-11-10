import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.clear()
g.size(), g.sizeEdges()
# (0, 0)
