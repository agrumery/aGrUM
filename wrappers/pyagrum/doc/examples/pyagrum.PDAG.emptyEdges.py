import pyagrum as gum

pdag = gum.fastPDAG("A->B->C")
pdag.emptyEdges()
# True
pdag.addEdge(0, 2)
pdag.emptyEdges()
# False
