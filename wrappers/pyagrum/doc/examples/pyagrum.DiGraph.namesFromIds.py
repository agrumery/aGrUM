import pyagrum as gum

g = gum.fastDiGraph("A->B->C->A")
sorted(g.namesFromIds(g.arcs()))  # a set of arcs (tuples of ids) gives a set of tuples of names
# [('A', 'B'), ('B', 'C'), ('C', 'A')]
