import pyagrum as gum

g = gum.fastDiGraph("A->B->C")
g.eraseArc(0, 1)
sorted(g.arcs())
# [(1, 2)]
