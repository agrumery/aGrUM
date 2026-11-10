import pyagrum as gum

g = gum.fastDiGraph("A->B->C;D->C")
g.ancestors(2)  # the ancestors of C
# {0, 1, 3}
g.ancestors(0)
# set()
