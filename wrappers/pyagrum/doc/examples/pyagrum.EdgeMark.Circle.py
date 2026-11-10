import pyagrum as gum

g = gum.MarkedGraph()
g.addNodes(2)
g.addEdge(0, 1, gum.EdgeMark.Circle, gum.EdgeMark.Arrowhead)  # 0o->1
g.markAt(1, 0)  # the mark at 0 is not determined
# <EdgeMark.Circle: 0>
g.isCircle(1, 0)
# True
