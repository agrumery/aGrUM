import pyagrum as gum

cg = gum.CliqueGraph()
c1 = cg.addNode({0, 1})
c2 = cg.addNode({1, 2})
cg.addEdge(c1, c2)
cg.separator(c1, c2)
# {1}
