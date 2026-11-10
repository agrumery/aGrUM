import pyagrum as gum

cg = gum.CliqueGraph()
cg.addNode({0, 1})
# 0
cg.addNode({1, 2})
# 1
cg.container(2)  # the clique containing the node 2
# 1
