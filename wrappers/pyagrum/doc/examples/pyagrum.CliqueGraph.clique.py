import pyagrum as gum

jt = gum.JunctionTreeGenerator().junctionTree(gum.fastBN("A->B->C"))
sorted(sorted(jt.clique(c)) for c in jt.nodes())
# [[0, 1], [1, 2]]
