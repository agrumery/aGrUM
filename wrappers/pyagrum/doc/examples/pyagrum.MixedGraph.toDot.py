import pyagrum as gum

g = gum.fastMixedGraph("A->B-C")
dot = g.toDot()
dot.startswith("digraph")
# True
"0 -> 1" in dot
# True
