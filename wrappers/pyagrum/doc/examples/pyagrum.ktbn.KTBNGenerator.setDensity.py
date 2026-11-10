import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
gen.setDensity(0.3)  # 30% of the legal arcs
round(gen.generate().sizeArcs() / gen.nbLegalArcs(), 1)
# 0.3
