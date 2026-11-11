import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1<-C"))
model.fillCPT("C", ktbn.KTBN.ATEMPORAL, {}, [0.4, 0.6])  # P(C), no parents
model.fillCPT("X", 1, {("X", 0): 0, ("C", ktbn.KTBN.ATEMPORAL): 1}, [0.3, 0.7])
model.fillCPT("X[1]", {"X[0]": "1", "C": 1}, [0.6, 0.4])  # engine names, a label ("1") or an index (1)
model.cpt("X", 1)[{"X[0]": 0, "C": 1}].tolist()
# [0.3, 0.7]
model.cpt("X", 1)[{"X[0]": 1, "C": 1}].tolist()
# [0.6, 0.4]
model.cpt("C", ktbn.KTBN.ATEMPORAL).tolist()
# [0.4, 0.6]
