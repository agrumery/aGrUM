import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B;C->D;E"))
cpdag.connectedComponentsCount()
# 3
