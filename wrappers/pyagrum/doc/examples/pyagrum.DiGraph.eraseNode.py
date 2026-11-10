import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.eraseNode(1)
g.nodes()
# {0, 2}
g.sizeArcs()
# 0
