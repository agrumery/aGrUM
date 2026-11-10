import pyagrum as gum

bn = gum.fastBN("A[3]->B")
bnmin, bnmax = gum.BayesNet(bn), gum.BayesNet(bn)
bnmin.cpt("A").fillWith([0.1, 0.2, 0.3])
bnmax.cpt("A").fillWith([0.3, 0.4, 0.5])
cn = gum.CredalNet(bnmin, bnmax)
cn.intervalToCredal()
cn.approximatedBinarization()  # creates indicator variables A-v0, A-v1, A-v2
gum.CredalNet.NodeType_Indic
# 3
bnb = cn.current_bn()
sorted(bnb.variable(n).name() for n in bnb.nodes() if cn.currentNodeType(n) == gum.CredalNet.NodeType_Indic)
# ['A-v0', 'A-v1', 'A-v2']
