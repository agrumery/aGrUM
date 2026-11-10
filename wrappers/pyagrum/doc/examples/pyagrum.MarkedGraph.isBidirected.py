import pyagrum as gum

g = gum.fastMarkedGraph("A<->B;B->C")
g.isBidirected(0, 1), g.isBidirected(1, 2)
# (True, False)
