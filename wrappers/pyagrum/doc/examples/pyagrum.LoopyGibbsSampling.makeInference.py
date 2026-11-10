import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyGibbsSampling(bn)
ie.setEpsilon(1e-2)
ie.setEvidence({"B": 1})
ie.makeInference()
abs(ie.posterior("A")[0] - 0.0345) < 0.05  # sampling: close to the exact 0.0345
# True
