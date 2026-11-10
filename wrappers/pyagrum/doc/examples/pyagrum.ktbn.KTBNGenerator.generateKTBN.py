import pyagrum as gum
import pyagrum.ktbn as ktbn

gum.initRandom(42)
gen = ktbn.KTBNGenerator(2, 3, 1)  # k=2, 3 processes, 1 atemporal variable
model = ktbn.KTBN(2)
gen.generateKTBN(model)
model.size()  # 3*2 + 1 nodes
# 7
