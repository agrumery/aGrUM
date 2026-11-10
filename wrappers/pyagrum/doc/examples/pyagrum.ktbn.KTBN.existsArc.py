import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.existsArc("X", 0, "X", 1)
# True
model.existsArc("X[1]", "X[0]")
# False
