import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveXDSL("model.xdsl")
bn2 = gum.BayesNet()
bn2.loadXDSL("model.xdsl")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
