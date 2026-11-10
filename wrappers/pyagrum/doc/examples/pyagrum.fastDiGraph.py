import pyagrum as gum

g = gum.fastDiGraph("A->B->C->A")  # cycles are allowed in a DiGraph
sorted(g.arcs())
# [(0, 1), (1, 2), (2, 0)]
