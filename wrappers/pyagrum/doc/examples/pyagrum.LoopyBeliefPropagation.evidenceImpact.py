import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
ie = gum.LoopyBeliefPropagation(bn)
p = ie.evidenceImpact("B", ["A"])  # P(B|A) for every value of A
p.names
# ('B', 'A')
round(p[{"A": 1, "B": 1}], 4)
# 0.7
