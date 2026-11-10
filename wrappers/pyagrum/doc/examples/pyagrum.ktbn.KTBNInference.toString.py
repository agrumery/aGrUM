import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.addObservation("X", 2, 1)
ie.makeInference(3)
print(ie.toString())
# KTBNInference (k=2, interface=1)
#   interventions: {}
#   observations: {X[2]}
#   targets: <all bases>
#   state: computed
