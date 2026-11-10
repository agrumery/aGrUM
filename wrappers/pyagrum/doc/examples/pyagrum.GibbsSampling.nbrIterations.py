import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.GibbsSampling(bn)
ie.setMaxIter(1000)  # stops at the first period after 1000 samples
ie.setEpsilon(1e-9)
ie.setMinEpsilonRate(1e-12)
ie.makeInference()
ie.nbrIterations()
# 1000
