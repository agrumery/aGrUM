import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.nameFromId(3)
# 'D'
