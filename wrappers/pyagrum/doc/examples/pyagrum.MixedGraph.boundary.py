import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;B->D")
g.boundary(1)  # parents, children and neighbours of B
# {0, 2, 3}
