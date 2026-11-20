import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.addObservation("X", 1, 1)
ie.makeInference(2)
round(ie.logObservationProbability(), 4)  # deprecated, use log2ObservationProbability: ln P(X[1] = 1) = ln 0.45
# -0.7985
