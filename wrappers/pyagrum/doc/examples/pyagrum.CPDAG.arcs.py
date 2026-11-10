import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
sorted(cpdag.arcs())  # the compelled arcs
# [(1, 2), (3, 2)]
