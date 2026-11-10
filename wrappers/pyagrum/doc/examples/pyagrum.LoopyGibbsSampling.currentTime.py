import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyGibbsSampling(bn)
ie.setEpsilon(1e-2)
ie.makeInference()
ie.currentTime() >= 0  # elapsed seconds (not deterministic)
# True
