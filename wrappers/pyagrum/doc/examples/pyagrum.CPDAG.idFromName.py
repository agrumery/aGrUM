import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.idFromName("D")
# 3
cpdag.idFromName("Z") is None
# True
