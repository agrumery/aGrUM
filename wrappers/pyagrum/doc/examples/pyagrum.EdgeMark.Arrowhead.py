import pyagrum as gum

g = gum.MarkedGraph()
g.addNodes(2)
g.addEdge(0, 1, gum.EdgeMark.Tail, gum.EdgeMark.Arrowhead)  # 0->1
g.markAt(0, 1)  # the mark at 1
# <EdgeMark.Arrowhead: 2>
g.isArrowhead(0, 1)
# True
