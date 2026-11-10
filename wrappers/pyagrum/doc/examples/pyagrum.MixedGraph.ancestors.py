import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D")
g.ancestors(3)  # following the arcs only
# {2}
g.ancestors(1)
# {0}
