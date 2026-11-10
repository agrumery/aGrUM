import pyagrum as gum

g = gum.fastMixedGraph("A->B-C-D")
sorted(g.edges())
# [(1, 2), (2, 3)]
