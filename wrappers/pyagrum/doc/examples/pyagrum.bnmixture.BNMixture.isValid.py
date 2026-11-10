import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BNMixture()
mix.add("bn1", gum.fastBN("A->B->C"), w=0.6)
mix.add("bn2", gum.fastBN("A->B;A->C"), w=0.4)
mix.isValid()
# True
mix.setWeight("bn1", 0)
mix.setWeight("bn2", 0)
mix.isValid()  # all the weights are 0
# False
