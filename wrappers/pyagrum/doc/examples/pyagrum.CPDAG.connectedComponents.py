import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B;C->D"))
cc = cpdag.connectedComponents()
cc[0] == cc[1], cc[0] == cc[2]
# (True, False)
