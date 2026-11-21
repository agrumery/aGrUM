import pyagrum as gum

g = gum.fastDiGraph("A->B->C->A")
g.idsFromNames([("C", "A"), ("A", "B")])
# [(2, 0), (0, 1)]
