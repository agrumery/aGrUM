import pyagrum as gum

cg = gum.CliqueGraph()
c1, c2, c3 = cg.addNode({0, 1}), cg.addNode({2}), cg.addNode({1, 3})
cg.addEdge(c1, c2)
cg.addEdge(c2, c3)
cg.hasRunningIntersection()  # 1 is in c1 and c3 but not in c2
# False
cg.addToClique(c2, 1)
cg.hasRunningIntersection()
# True
