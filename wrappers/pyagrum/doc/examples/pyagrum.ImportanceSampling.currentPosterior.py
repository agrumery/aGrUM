import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.ImportanceSampling(bn)
ie.setEpsilon(1e-2)
ie.makeInference()
ie.currentPosterior("B").names  # during inference (e.g. in a listener), it gives the current estimate
# ('B',)
ie.currentPosterior("B").tolist() == ie.posterior("B").tolist()  # after inference: the posterior
# True
