import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D")
g.descendants(2)  # following the arcs only
# {3}
g.descendants(0)
# {1}
