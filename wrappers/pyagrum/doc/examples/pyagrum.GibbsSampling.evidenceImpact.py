import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.GibbsSampling(bn)
ie.setEpsilon(1e-2)
p = ie.evidenceImpact("B", ["A"])  # P(B|A) for every value of A
p.names
# ('B', 'A')
abs(p[{"A": 1, "B": 1}] - 0.7) < 0.05  # sampling: close to the exact 0.7
# True
