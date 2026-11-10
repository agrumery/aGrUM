import pyagrum as gum

g = gum.fastDiGraph("A->B->C;A->C")
sorted(g.arcs())
# [(0, 1), (0, 2), (1, 2)]
