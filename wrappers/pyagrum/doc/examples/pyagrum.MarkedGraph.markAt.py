import pyagrum as gum

g = gum.fastMarkedGraph("A o-> B")
g.markAt(0, 1)  # the mark at B
# <EdgeMark.Arrowhead: 2>
g.markAt(1, 0)  # the mark at A
# <EdgeMark.Circle: 0>
