import pyagrum as gum

g = gum.fastDiGraph("A->B")
g.existsNode(1)
# True
g.existsNode(5)
# False
