import pyagrum as gum

g = gum.fastDiGraph("A->B")
g.existsArc(0, 1)
# True
g.existsArc(1, 0)
# False
