import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
pdag.existsNode(2), pdag.existsNode(3)
# (True, False)
