import pyagrum as gum

cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->C<-B;C->D"))
cpdag.idsFromNames(frozenset({"D"}))
# frozenset({3})
