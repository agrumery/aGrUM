import pyagrum as gum

pdag = gum.fastPDAG("A->B-C<-D")
# deprecated: use pyagrum.ChainGraph.cSeparation
pdag.cSeparation(0, 3)
# True
pdag.cSeparation(0, 3, {1, 2})
# False
