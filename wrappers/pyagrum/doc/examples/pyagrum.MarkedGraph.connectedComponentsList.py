import pyagrum as gum

g = gum.fastMarkedGraph("A->B;C<->D")
sorted(sorted(c) for c in g.connectedComponentsList().values())
# [[0, 1], [2, 3]]
