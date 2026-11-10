import pyagrum as gum

g = gum.fastMarkedGraph("A->B;C<->D;E")
g.connectedComponentsCount()
# 3
