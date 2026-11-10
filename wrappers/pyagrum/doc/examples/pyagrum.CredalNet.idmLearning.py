import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([3, 7])  # counts of events
bn.cpt("B")[:] = [[1, 9], [5, 5]]
cn = gum.CredalNet(bn)
cn.idmLearning(2)
[[round(p, 4) for p in v] for v in cn.credalNet_currentCpt()[bn.idFromName("A")][0]]
# [[0.4167, 0.5833], [0.25, 0.75]]
