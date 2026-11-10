import pyagrum as gum

gum.initRandom(42)
bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
ie = gum.LoopyMonteCarloSampling(bn)
ie.setEpsilon(1e-3)
ie.makeInference()
abs(ie.posterior("B")[0] - 0.42) < 0.05
# True
bn.cpt("A").fillWith([0.7, 0.3])  # changed in place: the inference is not notified
ie.invalidateTensors()
ie.makeInference()
abs(ie.posterior("B")[0] - 0.72) < 0.05  # 0.7 * 0.9 + 0.3 * 0.3
# True
