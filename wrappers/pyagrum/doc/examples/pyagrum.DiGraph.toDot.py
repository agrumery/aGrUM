import pyagrum as gum

g = gum.fastDiGraph("A->B")
dot = g.toDot()
dot.startswith("digraph")
# True
"0 -> 1" in dot
# True
