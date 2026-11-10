import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C"))
jt.size()
# 2
jt.clear()
jt.size(), jt.sizeEdges()
# (0, 0)
