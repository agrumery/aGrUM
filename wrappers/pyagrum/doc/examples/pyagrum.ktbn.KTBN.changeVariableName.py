import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
model.changeVariableName("X", "Y")
model.nodes()
# (('C', -1), ('Y', 0), ('Y', 1))
