import pyagrum as gum

g = gum.fastUndiGraph("A-B-C")
g.eraseNode(1)
g.nodes()
# {0, 2}
g.sizeEdges()
# 0
