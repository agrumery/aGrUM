import pyagrum as gum
import pyagrum.bnmixture as bnm

bn = gum.fastBN("A->B->C")
mix = bnm.BootstrapMixture("ref", bn)
mix.add("bn1", bn)
mix.add("bn2", bn)  # same BN: bn1 now has weight 2
mix.add("bn3", gum.fastBN("A->B;A->C"))
mix.existsArc("B", "C")  # number of BNs with the arc B->C
# 1
round(mix.arcConfidence("B", "C"), 4)  # 2 of the 3 copies
# 0.6667
