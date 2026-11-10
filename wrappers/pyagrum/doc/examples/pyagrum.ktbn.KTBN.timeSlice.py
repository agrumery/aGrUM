import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.timeSlice(model.variable("X[1]"))
# 1
model.timeSlice(model.variable("C"))
# -1
