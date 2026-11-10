import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyImportanceSampling(bn)
ie.setEpsilon(1e-2)
ie.makeInference()
ie.messageApproximationScheme().startswith("stopped with")  # e.g. 'stopped with epsilon=0.01'
# True
