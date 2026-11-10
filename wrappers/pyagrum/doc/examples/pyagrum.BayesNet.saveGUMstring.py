import pyagrum as gum

bn = gum.fastBN("A->B")
s = bn.saveGUMstring(indent=-1)  # a compact jgum (JSON) string
s.startswith("{")
# True
bn2 = gum.BayesNet()
bn2.loadGUMstring(s)
bn2.hasSameStructure(bn)
# True
