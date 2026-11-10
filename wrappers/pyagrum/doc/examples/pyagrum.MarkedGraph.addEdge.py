import pyagrum as gum

g = gum.MarkedGraph()
g.addNodes(3)
g.addEdge(0, 1)  # o-o edge: both marks are circles
g.addEdge(1, 2, gum.EdgeMark.Tail, gum.EdgeMark.Arrowhead)  # 1->2
print(g)
# {0,1,2} , {0 o-o 1, 1 --> 2}
