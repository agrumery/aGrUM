import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
print(model.toString())
# k-TBN (k=2, 3 nodes, 2 arcs)
#   temporal processes (1): X
#   atemporal variables (1): C
#
#   arcs (2):
#     X[0] -> X[1]
#     C -> X[1]
