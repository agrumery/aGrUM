import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
cpdag.edges()  # the reversible arcs
# {(0, 1)}
