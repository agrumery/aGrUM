import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.hasDirectedPath(0, 2)
# True
g.hasDirectedPath(2, 0)
# False
