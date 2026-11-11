import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)  # reproducible
gen = ktbn.KTBNGenerator(3, 4, 1)  # k=3, 4 temporal processes, 1 atemporal variable
gen.setDensity(0.15).setMaxParents(4)
model = gen.generate()
model.k()
# 3
sorted(model.temporalVarNames())
# ['X0', 'X1', 'X2', 'X3']
sorted(model.atemporalVarNames())
# ['A0']
