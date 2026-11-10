import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.LazyPropagation(bn)
ie.makeInference()
jt = ie.junctionTree()
sorted(sorted(jt.clique(c)) for c in jt.nodes())
# [[0, 1], [1, 2]]
jt.sizeEdges()
# 1
