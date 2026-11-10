import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
mix.isValid()
# True
mix.setWeight("bn1", 0)
mix.setWeight("bn2", 0)
mix.isValid()  # all the weights are 0
# False
