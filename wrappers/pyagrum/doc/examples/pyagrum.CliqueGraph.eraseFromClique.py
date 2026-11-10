import pyagrum as gum

cg = gum.CliqueGraph()
c = cg.addNode({0, 1, 2})
cg.eraseFromClique(c, 1)
cg.clique(c)
# {0, 2}
