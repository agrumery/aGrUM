import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
gen.setDensity(0.5).setMaxParents(1)
model = gen.generate()
max(len(model.parents(*node)) for node in model.nodes())
# 1
