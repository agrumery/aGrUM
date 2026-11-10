import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveNET("model.net")
bn2 = gum.BayesNet()
bn2.loadNET("model.net")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
