import pyagrum as gum

bn = gum.fastBN("A->B->C")
ie = gum.ShaferShenoyInference(bn)
ie.makeInference()
jt = ie.joinTree()
sorted(sorted(jt.clique(c)) for c in jt.nodes())
# [[0, 1], [1, 2]]
