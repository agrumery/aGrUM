import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C"))
jt.clearEdges()  # the cliques are kept
jt.size(), jt.sizeEdges()
# (2, 0)
