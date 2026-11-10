import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.clear()
g.size(), g.sizeArcs()
# (0, 0)
