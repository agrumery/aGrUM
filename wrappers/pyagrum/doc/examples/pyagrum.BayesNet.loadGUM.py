import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveGUM("model.bgum")
bn2 = gum.BayesNet()
bn2.loadGUM("model.bgum")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
