import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
mix.setWeight("bn2", 0.9)
mix.weight("bn2")
# 0.9
