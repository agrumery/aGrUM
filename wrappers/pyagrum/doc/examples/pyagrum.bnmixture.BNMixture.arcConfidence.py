import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BNMixture()
mix.add("bn1", gum.fastBN("A->B->C"), w=0.6)
mix.add("bn2", gum.fastBN("A->B;A->C"), w=0.4)
mix.arcConfidence("A", "B")  # weighted frequency of the arc A->B
# 1.0
mix.arcConfidence("B", "C")
# 0.6
