import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BNMixture()
mix.add("bn1", gum.fastBN("A->B->C"), w=0.6)
mix.add("bn2", gum.fastBN("A->B;A->C"), w=0.4)
sorted(mix.refBN.arcs())  # the reference is a copy of bn1 (highest weight)
# [(0, 1), (1, 2)]
mix.setWeight("bn2", 0.9)
mix.updateRef()
sorted(mix.refBN.arcs())  # now a copy of bn2
# [(0, 1), (0, 2)]
