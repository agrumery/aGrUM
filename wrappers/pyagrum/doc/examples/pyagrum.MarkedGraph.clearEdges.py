import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.clearEdges()
g.size(), g.sizeEdges()
# (3, 0)
