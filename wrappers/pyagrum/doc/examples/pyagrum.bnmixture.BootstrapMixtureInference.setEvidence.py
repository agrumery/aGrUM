import pyagrum as gum
import pyagrum.bnmixture as bnm

bn = gum.fastBN("A->B->C")
bn.cpt("A").fillWith([0.5, 0.5])
mix = bnm.BootstrapMixture("ref", bn)
for i, p in enumerate([0.4, 0.45, 0.55, 0.6]):  # BNs that differ by the prior of A
  other = gum.BayesNet(bn)
  other.cpt("A").fillWith([p, 1 - p])
  mix.add(f"bn{i}", other)
ie = bnm.BootstrapMixtureInference(mix)
ie.setEvidence({"A": 0})
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [1.0, 0.0]
