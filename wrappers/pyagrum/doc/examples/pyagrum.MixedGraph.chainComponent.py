import pyagrum as gum

g = gum.fastMixedGraph("A->B-C-D;D->E")
g.chainComponent(2)
# {1, 2, 3}
g.chainComponent(0)
# {0}
