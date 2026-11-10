import pyagrum as gum

pdag = gum.fastPDAG("A->B-C;C->D")
pdag.mixedUnorientedPath(3, 0)  # the arcs may be followed backwards
# [3, 2, 1, 0]
