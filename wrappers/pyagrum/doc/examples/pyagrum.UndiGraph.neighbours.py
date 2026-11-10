import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;B-D")
g.neighbours(1)
# {0, 2, 3}
