import pyagrum as gum
import pyagrum.bnmixture as bnm

bn1 = gum.fastBN("A->B->C")
bn2 = gum.fastBN("A->B->C")
bn1.cpt("A").fillWith([0.2, 0.8])
bn2.cpt("A").fillWith([0.6, 0.4])
mix = bnm.BNMixture()
mix.add("bn1", bn1, w=0.5)
mix.add("bn2", bn2, w=0.5)
ie = bnm.BNMixtureInference(mix)
ie.setEvidence({"A": 1})
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.0, 1.0]
