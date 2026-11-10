import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C->D")
# deprecated: use pyagrum.ChainGraph.hasMixedReallyOrientedPath
pdag.hasMixedReallyOrientedPath(0, 3)
# True
pdag.hasMixedReallyOrientedPath(1, 2)  # B-C: no arc on the walk
# False
