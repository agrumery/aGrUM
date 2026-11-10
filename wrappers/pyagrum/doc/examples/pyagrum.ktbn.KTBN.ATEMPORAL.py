import pyagrum as gum
import pyagrum.ktbn as ktbn

ktbn.KTBN.ATEMPORAL  # the slice of an atemporal variable
# -1
model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))
model.parents("X", 1)
# (('X', 0), ('C', -1))
