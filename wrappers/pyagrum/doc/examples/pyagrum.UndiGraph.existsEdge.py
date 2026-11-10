import pyagrum as gum

g = gum.fastUndiGraph("A-B")
g.existsEdge(0, 1)
# True
g.existsEdge(1, 0)  # no orientation
# True
