import pyagrum as gum

g = gum.fastUndiGraph("A-B")
dot = g.toDot()
dot.startswith("graph")
# True
"0 -- 1" in dot
# True
