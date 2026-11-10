import pyagrum as gum

g = gum.fastMixedGraph("A->B-C;C->D-E")
cc = g.chainComponents()  # node id -> chain component
cc[1] == cc[2], cc[3] == cc[4], cc[0] == cc[1], cc[2] == cc[3]
# (True, True, False, False)
