import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))
model.fillCPT("X", 1, {("X", 0): 0, ("C", ktbn.KTBN.ATEMPORAL): 1}, [0.3, 0.7])
model.fillCPT("X[1]", {"X[0]": 1, "C": 1}, [0.6, 0.4])  # with engine names
model.cpt("X", 1)[{"X[0]": 0, "C": 1}].tolist()
# [0.3, 0.7]
