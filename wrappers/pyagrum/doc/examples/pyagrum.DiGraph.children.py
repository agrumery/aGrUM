import pyagrum as gum

g = gum.fastDiGraph("A->B;A->C;C->D")
g.children(0)
# {1, 2}
g.children(3)
# set()
