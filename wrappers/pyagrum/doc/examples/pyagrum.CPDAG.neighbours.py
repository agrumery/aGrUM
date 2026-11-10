import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.neighbours(1)  # the arc B->C is not counted
# {0}
