import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.saveBIFXML("model.bifxml")
bn2 = gum.BayesNet()
bn2.loadBIFXML("model.bifxml")
sorted(bn2.names())
# ['A', 'B', 'C']
bn2.hasSameStructure(bn)
# True
