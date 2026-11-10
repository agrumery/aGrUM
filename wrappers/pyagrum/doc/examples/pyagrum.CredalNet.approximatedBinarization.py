import pyagrum as gum

bn = gum.fastBN("A[3]->B")
bnmin, bnmax = gum.BayesNet(bn), gum.BayesNet(bn)
bnmin.cpt("A").fillWith([0.1, 0.2, 0.3])
bnmax.cpt("A").fillWith([0.3, 0.4, 0.5])
cn = gum.CredalNet(bnmin, bnmax)
cn.intervalToCredal()
cn.approximatedBinarization()
sorted(cn.current_bn().names())  # A is replaced by binary variables
# ['A-b0', 'A-b1', 'A-v0', 'A-v1', 'A-v2', 'B']
