import pyagrum as gum

bn = gum.fastBN("A->B")
bnmin, bnmax = gum.BayesNet(bn), gum.BayesNet(bn)
bnmin.cpt("A").fillWith([0, 0])
bnmax.cpt("A").fillWith([1, 1])  # no knowledge at all about A
cn = gum.CredalNet(bnmin, bnmax)
cn.intervalToCredal()
gum.CredalNet.NodeType_Vacuous
# 2
cn.nodeType(bn.idFromName("A")) == gum.CredalNet.NodeType_Vacuous
# True
