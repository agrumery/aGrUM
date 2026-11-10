import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyImportanceSampling(bn)
ie.setEpsilon(1e-2)
ie.addEvidence("A", 1)
ie.makeInference()
round(abs(ie.H("A")), 4)  # A is observed
# 0.0
abs(ie.H("B") - 0.8813) < 0.05  # entropy of P(B|A=1)=[0.3, 0.7] is 0.8813
# True
