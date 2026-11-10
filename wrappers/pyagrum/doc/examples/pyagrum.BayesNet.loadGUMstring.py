import pyagrum as gum

bn = gum.fastBN("A->B->C")
s = bn.saveGUMstring()
bn2 = gum.BayesNet()
bn2.loadGUMstring(s)
bn2.hasSameStructure(bn)
# True
