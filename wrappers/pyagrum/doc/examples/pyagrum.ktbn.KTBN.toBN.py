import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
bn = model.toBN()
sorted(bn.names())
# ['C', 'X[0]', 'X[1]']
