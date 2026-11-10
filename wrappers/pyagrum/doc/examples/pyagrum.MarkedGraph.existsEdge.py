import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C")
g.existsEdge(1, 0)  # whatever the marks and the order of the nodes
# True
g.existsEdge(0, 2)
# False
