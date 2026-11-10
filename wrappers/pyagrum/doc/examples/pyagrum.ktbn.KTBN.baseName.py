import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
var = model.variable("X", 1)
var.name()
# 'X[1]'
model.baseName(var)
# 'X'
