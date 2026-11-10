import pyagrum as gum

cg = gum.CliqueGraph()
cg.addNode({0, 1})  # a new clique containing the nodes 0 and 1
# 0
cg.addNode()  # a new empty clique
# 1
cg.clique(0), cg.clique(1)
# ({0, 1}, set())
