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
ie.makeInference()
print(ie.posterior("A"))
#
#   A                │
# 0        │1        │
# ─────────│─────────│
#  0.5000  │ 0.5000  │
