import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.variable("X", 1).name()
# 'X[1]'
model.variable("C", ktbn.KTBN.ATEMPORAL).name()
# 'C'
