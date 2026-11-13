import pyagrum as gum

g = gum.fastDiGraph("A->B->C->A")  # cycles are allowed in a DiGraph
sorted(g.arcs())
# [(0, 1), (1, 2), (2, 0)]

g = gum.fastDiGraph("'Visit Asia'->'X-ray'")  # names between single quotes
sorted(g.nameFromId(n) for n in g.nodes())
# ['Visit Asia', 'X-ray']
