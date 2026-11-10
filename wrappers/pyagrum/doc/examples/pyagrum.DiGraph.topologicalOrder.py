import pyagrum as gum

g = gum.fastDiGraph("C->B->A;C->A")
[g.nameFromId(i) for i in g.topologicalOrder()]
# ['C', 'B', 'A']
