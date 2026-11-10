import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
sorted(mix.refBN.names())
# ['A', 'B', 'C']
sorted(mix.refBN.arcs())
# [(0, 1), (1, 2)]
