import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveDSL("model.dsl")
bn2 = gum.BayesNet()
bn2.loadDSL("model.dsl")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
