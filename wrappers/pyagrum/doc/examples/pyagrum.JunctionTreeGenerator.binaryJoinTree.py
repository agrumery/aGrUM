import pyagrum as gum

bn = gum.fastBN("A->D;B->D;C->D")
bjt = gum.JunctionTreeGenerator().binaryJoinTree(bn)
bjt.isJoinTree()
# True
max(len(bjt.neighbours(c)) for c in bjt.nodes()) <= 3  # each clique has at most 3 neighbours
# True
