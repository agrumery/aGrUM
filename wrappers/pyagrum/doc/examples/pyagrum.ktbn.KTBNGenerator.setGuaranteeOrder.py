import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(3, 2)
gen.setGuaranteeOrder(True)
model = gen.generate()
# at least one arc of lag k-1 = 2 into the last slice
any(tail[1] == 0 and head[1] == 2 for tail, head in model.arcs())
# True
