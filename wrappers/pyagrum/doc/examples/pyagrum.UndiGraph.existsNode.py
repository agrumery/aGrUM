import pyagrum as gum

g = gum.fastUndiGraph("A-B")
g.existsNode(1)
# True
g.existsNode(5)
# False
