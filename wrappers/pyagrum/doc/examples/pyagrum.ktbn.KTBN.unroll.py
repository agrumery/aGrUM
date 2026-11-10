import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
bn = model.unroll(4)
sorted(bn.names())
# ['C', 'X[0]', 'X[1]', 'X[2]', 'X[3]']
bn.sizeArcs()
# 6
