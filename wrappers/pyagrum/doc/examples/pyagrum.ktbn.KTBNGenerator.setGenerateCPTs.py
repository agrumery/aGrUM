import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
gen.setGenerateCPTs(False)  # the structure only
model = gen.generate()
model.cpt("X0", 0).sum()
# 0.0
