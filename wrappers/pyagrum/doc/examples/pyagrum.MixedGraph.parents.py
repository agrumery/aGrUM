import pyagrum as gum

g = gum.fastMixedGraph("A->C;B->C;C-D")
c = g.idFromName("C")
sorted(g.nameFromId(i) for i in g.parents(c))  # the neighbour D is not a parent
# ['A', 'B']
