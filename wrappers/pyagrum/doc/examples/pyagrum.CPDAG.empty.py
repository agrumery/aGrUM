import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B"))
cpdag.empty()
# False
