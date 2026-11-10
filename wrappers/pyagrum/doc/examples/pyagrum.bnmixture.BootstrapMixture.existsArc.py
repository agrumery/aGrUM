import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
mix.existsArc("A", "B")  # number of BNs with the arc A->B
# 2
mix.existsArc("B", "C")
# 1
