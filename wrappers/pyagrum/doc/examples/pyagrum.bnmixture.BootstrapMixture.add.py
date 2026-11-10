import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
mix.add("bn3", gum.fastBN("C->B->A"), w=1)
mix.names()
# ['bn1', 'bn2', 'bn3']
