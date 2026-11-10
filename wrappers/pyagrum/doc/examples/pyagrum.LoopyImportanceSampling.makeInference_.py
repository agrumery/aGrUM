import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
gum.initRandom(42)
ie = gum.LoopyImportanceSampling(bn)
ie.setEpsilon(1e-2)
ie.makeInference_()  # internal step of the inference: use makeInference() instead
abs(ie.posterior("B")[1] - 0.58) < 0.05  # sampling: close to the exact 0.58
# True
