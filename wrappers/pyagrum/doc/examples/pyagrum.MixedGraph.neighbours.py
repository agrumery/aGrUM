import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;B-D")
g.neighbours(1)  # the edges only
# {2, 3}
