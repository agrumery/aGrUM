import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.parents("X", 1)
# (('X', 0), ('C', -1))
model.parents("X[1]")
# (('X', 0), ('C', -1))
