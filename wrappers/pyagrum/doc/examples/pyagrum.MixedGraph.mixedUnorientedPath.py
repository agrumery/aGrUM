import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;D->C")
g.mixedUnorientedPath(0, 3)  # the arcs are followed in any direction
# [0, 1, 2, 3]
