import pyagrum as gum

g = gum.fastMarkedGraph("A o-> B")
g.isArrowhead(0, 1)  # the mark at B
# True
g.isArrowhead(1, 0)  # the mark at A
# False
