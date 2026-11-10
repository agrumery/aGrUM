import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
gen.setNamePrefixes("P", "S")
model = gen.generate()
sorted(model.temporalVarNames())
# ['P0', 'P1', 'P2']
model.atemporalVarNames()
# frozenset({'S0'})
