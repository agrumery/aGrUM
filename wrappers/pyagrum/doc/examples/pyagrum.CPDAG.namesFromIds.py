import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->C<-B;C->D"))
sorted(cpdag.namesFromIds(cpdag.arcs()))
# [('A', 'C'), ('B', 'C'), ('C', 'D')]
