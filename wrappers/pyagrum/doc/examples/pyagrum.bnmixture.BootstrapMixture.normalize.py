import pyagrum as gum
import pyagrum.bnmixture as bnm

mix = bnm.BootstrapMixture("ref", gum.fastBN("A->B->C"))
mix.add("bn1", gum.fastBN("A->B->C"))
mix.add("bn2", gum.fastBN("A->B;A->C"))
mix.add("bn3", gum.fastBN("C->B->A"), w=2)
mix.normalize()
{name: round(w, 4) for name, w in mix.weights().items()}
# {'bn1': 0.25, 'bn2': 0.25, 'bn3': 0.5}
