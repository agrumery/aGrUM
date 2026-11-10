import pyagrum as gum

g = gum.fastMixedGraph("A->C;B->C;C-D")
g.eraseParents(g.idFromName("C"))  # the edge C-D is kept
g.sizeArcs(), g.sizeEdges()
# (0, 1)
