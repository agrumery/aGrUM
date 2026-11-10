import pyagrum as gum

g = gum.MarkedGraph()
g.addNodes(2)
g.addEdge(0, 1, gum.EdgeMark.Tail, gum.EdgeMark.Arrowhead)  # 0->1
g.markAt(1, 0)  # the mark at 0
# <EdgeMark.Tail: 1>
g.isTail(1, 0)
# True
