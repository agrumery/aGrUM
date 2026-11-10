import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.existsEdge(1, 0), cpdag.existsEdge(1, 2)
# (True, False)
