import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BNMixture()
mix.add("bn1", gum.fastBN("A->B->C"), w=0.6)
mix.add("bn2", gum.fastBN("A->B;A->C"), w=0.4)
mix.isNormalized()  # 0.6 + 0.4 = 1
# True
mix.setWeight("bn1", 2)
mix.isNormalized()
# False
mix.normalize()
mix.isNormalized()
# True
