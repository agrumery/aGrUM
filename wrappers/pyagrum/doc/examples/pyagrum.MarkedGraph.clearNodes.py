import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.clearNodes()
g.size(), g.sizeEdges()
# (0, 0)
