import pyagrum as gum

g = gum.fastUndiGraph("A-B-C-D;A-E")
g.undirectedPath(4, 3)
# [4, 0, 1, 2, 3]
