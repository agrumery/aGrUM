import pyagrum as gum

pdag = gum.fastPDAG("A->B-C")
pdag.existsEdge(2, 1), pdag.existsEdge(0, 1)
# (True, False)
