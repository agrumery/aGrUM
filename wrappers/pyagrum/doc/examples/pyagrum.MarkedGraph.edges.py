import pyagrum as gum

g = gum.fastMarkedGraph("A->B<->C;C o-o D")
sorted(g.edges())
# [(0, 1), (1, 2), (2, 3)]
