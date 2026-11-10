import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.existsNode(3), cpdag.existsNode(4)
# (True, False)
