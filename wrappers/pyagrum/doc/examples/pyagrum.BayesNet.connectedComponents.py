import pyagrum as gum

bn = gum.fastBN("A->B;C->D;E")
cc = bn.connectedComponents()  # node id -> id of the root of its component
cc[0] == cc[1], cc[0] == cc[2]
# (True, False)
len(set(cc.values()))
# 3
