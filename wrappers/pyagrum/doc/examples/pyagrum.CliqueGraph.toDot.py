import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C"))
dot = jt.toDot()
dot.startswith("graph")
# True
