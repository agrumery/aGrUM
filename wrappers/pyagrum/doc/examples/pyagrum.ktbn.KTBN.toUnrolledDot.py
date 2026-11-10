import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))  # a 2-TBN: process X, atemporal C
dot = model.toUnrolledDot(4)  # 4 time slices
"X[3]" in dot
# True
