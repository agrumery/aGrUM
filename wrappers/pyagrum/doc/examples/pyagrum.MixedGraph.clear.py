import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.clear()
g.size(), g.sizeArcs(), g.sizeEdges()
# (0, 0, 0)
