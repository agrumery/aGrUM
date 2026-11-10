import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.existsArc(1, 2), cpdag.existsArc(0, 1)
# (True, False)
