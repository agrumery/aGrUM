import pyagrum as gum

g = gum.fastMarkedGraph("A o-> B")
g.isCircle(1, 0)  # the mark at A
# True
g.isCircle(0, 1)  # the mark at B
# False
