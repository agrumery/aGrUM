import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveBIF("model.bif")
bn2 = gum.BayesNet()
bn2.loadBIF("model.bif")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
