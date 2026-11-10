import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C;A->D"))
jt.isJoinTree()
# True
