import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
dot = model.bnToDot()  # the storage BN, no time slices
"X[0]" in dot
# True
