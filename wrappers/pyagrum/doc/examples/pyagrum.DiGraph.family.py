import pyagrum as gum

g = gum.fastDiGraph("A->C;B->C;C->D")
c = g.idFromName("C")
sorted(g.nameFromId(i) for i in g.family(c))  # C and its parents
# ['A', 'B', 'C']
