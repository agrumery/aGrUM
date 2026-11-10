import pyagrum as gum

g = gum.fastDiGraph("A->B;A->C;D->A")
g.eraseChildren(0)
sorted(g.arcs())
# [(3, 0)]
