import pyagrum as gum

g = gum.fastMarkedGraph("A->B")
g.existsNode(1), g.existsNode(2)
# (True, False)
