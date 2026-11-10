import pyagrum as gum

g = gum.fastMarkedGraph("A->B")
g.isTail(1, 0)  # the mark at A
# True
g.isTail(0, 1)  # the mark at B
# False
