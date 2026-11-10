import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C->D")
pdag.mixedOrientedPath(0, 3)  # the arcs in their direction
# [0, 1, 2, 3]
pdag.mixedOrientedPath(2, 1)  # the edges in either direction
# [2, 1]
