import pyagrum as gum

g = gum.fastMarkedGraph("A-B;B->C")
g.isUndirected(0, 1), g.isUndirected(1, 2)
# (True, False)
