import pyagrum as gum

g = gum.fastMarkedGraph("A->B;C<->D")
cc = g.connectedComponents()
cc[0] == cc[1], cc[0] == cc[2]
# (True, False)
