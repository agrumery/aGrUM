import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
ie = gum.LoopyBeliefPropagation(bn)
ie.makeInference()
ie.messageApproximationScheme()
# 'stopped with epsilon=1e-08'
