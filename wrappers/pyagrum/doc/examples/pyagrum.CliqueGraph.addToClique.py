import pyagrum as gum

cg = gum.CliqueGraph()
c = cg.addNode({0, 1})
cg.addToClique(c, 2)
cg.clique(c)
# {0, 1, 2}
