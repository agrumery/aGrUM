import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.dSeparation(0, 3)
# True
cpdag.dSeparation(0, 3, 2)  # C is a collider
# False
