import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BNMixture()
mix.add("bn1", gum.fastBN("A->B->C"), w=0.6)
mix.add("bn2", gum.fastBN("A->B;A->C"), w=0.4)
mix.add("bn3", gum.fastBN("C->B->A"), w=2)
mix.normalize()
{name: round(w, 4) for name, w in mix.weights().items()}
# {'bn1': 0.2, 'bn2': 0.1333, 'bn3': 0.6667}
