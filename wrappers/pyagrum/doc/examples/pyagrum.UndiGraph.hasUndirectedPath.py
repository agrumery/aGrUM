import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;D")
g.hasUndirectedPath(0, 2)
# True
g.hasUndirectedPath(0, 3)
# False
