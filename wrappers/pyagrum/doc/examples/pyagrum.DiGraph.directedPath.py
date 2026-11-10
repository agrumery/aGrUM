import pyagrum as gum

g = gum.fastDiGraph("A->B->C;A->D->C")
g.directedPath(0, 2)
# [0, 3, 2]
print(g.directedPath(2, 0))
# None
