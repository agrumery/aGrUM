import pyagrum as gum

pdag = gum.PDAG()
pdag.addNodeWithId(5)
pdag.nodes()
# {5}
pdag.existsNode(5), pdag.existsNode(0)
# (True, False)
