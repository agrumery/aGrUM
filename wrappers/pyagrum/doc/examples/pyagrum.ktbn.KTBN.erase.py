import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.erase("C")  # with its arcs
model.nodes()
# (('X', 0), ('X', 1))
model.arcs()
# ((('X', 0), ('X', 1)),)
