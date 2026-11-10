import pyagrum as gum

cg = gum.CliqueGraph()
c = cg.addNode({0, 1})
cg.setClique(c, {2, 3})
cg.clique(c)
# {2, 3}
