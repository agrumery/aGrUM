import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
sorted(cpdag.arcs()), sorted(cpdag.edges())  # B->C<-D compelled, A--B reversible
# ([(1, 2), (3, 2)], [(0, 1)])
