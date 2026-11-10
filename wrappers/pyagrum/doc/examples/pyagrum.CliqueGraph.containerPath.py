import pyagrum as gum

cg = gum.CliqueGraph()
c1, c2, c3 = cg.addNode({0, 1}), cg.addNode({1, 2}), cg.addNode({2, 3})
cg.addEdge(c1, c2)
cg.addEdge(c2, c3)
cg.containerPath(0, 3)  # from a clique containing 0 to one containing 3
# [0, 1, 2]
