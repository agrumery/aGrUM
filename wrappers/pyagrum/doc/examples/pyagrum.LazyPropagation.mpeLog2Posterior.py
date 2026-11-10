import pyagrum as gum

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.2, 0.8])
bn.cpt("B").fillWith([0.9, 0.1, 0.3, 0.7])
bn.cpt("C").fillWith([0.6, 0.4, 0.1, 0.9])
ie = gum.LazyPropagation(bn)
ie.setEvidence({"C": 0})
inst, log2p = ie.mpeLog2Posterior()
sorted(inst.todict().items())
# [('A', 1), ('B', 0), ('C', 0)]
round(log2p, 4)
# -1.1062
round(2**log2p, 4)  # the posterior probability of the MPE
# 0.4645
