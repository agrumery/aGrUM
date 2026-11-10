import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D")
g.mixedOrientedPath(0, 3)
# [0, 1, 2, 3]
print(g.mixedOrientedPath(3, 0))
# None
