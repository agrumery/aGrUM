import pyagrum as gum

bn = gum.fastBN("A->B")
bnmin, bnmax = gum.BayesNet(bn), gum.BayesNet(bn)
bnmin.cpt("A").fillWith([0.2, 0.6])
bnmax.cpt("A").fillWith([0.4, 0.8])
bnmin.cpt("B")[:] = [[0.1, 0.7], [0.5, 0.3]]
bnmax.cpt("B")[:] = [[0.3, 0.9], [0.7, 0.5]]
cn = gum.CredalNet(bnmin, bnmax)
cn.intervalToCredal()
cn.computeBinaryCPTMinMax()
ie = gum.CNLoopyPropagation(cn)
ie.makeInference()
ie.messageApproximationScheme()
# 'stopped on request'
