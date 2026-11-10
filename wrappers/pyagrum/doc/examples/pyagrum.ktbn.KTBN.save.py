import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.save("model.bgum")  # .jgum for a text file
ktbn.KTBN.load("model.bgum").size()
# 3
