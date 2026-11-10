import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyImportanceSampling(bn)
ie.setEpsilon(1e-2)
p = ie.evidenceImpact("A", ["B"])  # P(A|B) for every value of B
p.names
# ('A', 'B')
abs(p[{"A": 1, "B": 0}] - 0.5714) < 0.05  # sampling: close to the exact 0.24/0.42
# True
abs(p[{"A": 1, "B": 1}] - 0.9655) < 0.05  # sampling: close to the exact 0.56/0.58
# True
