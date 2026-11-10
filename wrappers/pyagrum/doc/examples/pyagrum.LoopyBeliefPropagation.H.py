import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
ie = gum.LoopyBeliefPropagation(bn)
ie.addEvidence("A", 1)
ie.makeInference()
round(abs(ie.H("A")), 4)  # A is observed
# 0.0
round(ie.H("B"), 4)  # entropy of P(B|A=1)=[0.3, 0.7]
# 0.8813
