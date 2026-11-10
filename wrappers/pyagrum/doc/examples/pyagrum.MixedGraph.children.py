import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;A->C")
g.children(0)
# {1, 2}
