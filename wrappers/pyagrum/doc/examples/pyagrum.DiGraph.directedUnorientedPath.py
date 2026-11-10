import pyagrum as gum

g = gum.fastDiGraph("A->B<-C")
g.directedUnorientedPath(0, 2)  # arcs followed in any direction
# [0, 1, 2]
print(g.directedPath(0, 2))
# None
