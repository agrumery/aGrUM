import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
gen.setDomainRange(3, 4)
model = gen.generate()
{model.variable(*node).domainSize() for node in model.nodes()} <= {3, 4}
# True
