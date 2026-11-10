import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
g.existsNode(2)
# True
g.existsNode(3)
# False
