import pyagrum as gum

g = gum.fastDiGraph("A->B->C;A->D")
g.descendants(0)  # the descendants of A
# {1, 2, 3}
g.descendants(2)
# set()
