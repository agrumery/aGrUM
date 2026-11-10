import pyagrum as gum

bn = gum.fastBN("A->B")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B")[:] = [[0.9, 0.1], [0.3, 0.7]]
ie = gum.LoopyBeliefPropagation(bn)
ie.setVerbosity(True)  # needed to record the history
ie.makeInference()
len(ie.history()) > 0  # one value per period
# True
