import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
model = gen.generate()
model.k()
# 2
sorted(model.temporalVarNames())
# ['X0', 'X1', 'X2']
model.atemporalVarNames()
# frozenset({'A0'})
