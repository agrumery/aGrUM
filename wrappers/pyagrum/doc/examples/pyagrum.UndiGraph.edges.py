import pyagrum as gum

g = gum.fastUndiGraph("A-B-C;A-C")
sorted(g.edges())
# [(0, 1), (0, 2), (1, 2)]
