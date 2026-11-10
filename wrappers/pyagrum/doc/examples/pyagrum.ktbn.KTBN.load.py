import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.save("model.jgum")
loaded = ktbn.KTBN.load("model.jgum")
loaded.arcs()
# ((('X', 0), ('X', 1)), (('C', -1), ('X', 1)))
