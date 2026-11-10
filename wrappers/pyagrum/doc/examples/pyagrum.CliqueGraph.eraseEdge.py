import pyagrum as gum

cg = gum.CliqueGraph()
c1, c2 = cg.addNode({0, 1}), cg.addNode({1, 2})
cg.addEdge(c1, c2)
cg.eraseEdge(gum.Edge(c1, c2))
cg.sizeEdges()
# 0
