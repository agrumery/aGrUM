import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0, 10])  # counts of events, with a zero
bn.cpt("B")[:] = [[0, 4], [5, 5]]
cn = gum.CredalNet(bn)
cn.lagrangeNormalization()
cn.idmLearning(0)
[round(p, 4) for p in cn.credalNet_currentCpt()[bn.idFromName("A")][0][0]]
# [0.0833, 0.9167]
