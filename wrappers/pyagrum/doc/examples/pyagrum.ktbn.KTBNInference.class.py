import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1->Y1;Y0"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
model.cpt("Y", 0).fillWith([0.5, 0.5])
model.cpt("Y", 1).fillWith([0.8, 0.2, 0.3, 0.7])  # P(Y[t] | X[t])
ie = ktbn.KTBNInference(model)
ie.addObservation("Y", 3, 1)  # observe Y[3] = 1
ie.addIntervention("X", 1, 1)  # do(X[1] = 1)
ie.addTarget("X")  # the whole series P(X[t] | ...)
ie.makeInference(5)  # slices 0..4
[[round(p, 4) for p in post.tolist()] for post in ie.posteriors("X")]  # X[0] is not revised: do(X[1]) cuts it off
# [[0.5, 0.5], [0.0, 1.0], [0.0943, 0.9057], [0.1283, 0.8717], [0.2898, 0.7102]]
round(ie.logObservationProbability(), 4)  # log P(Y[3] = 1 | do(X[1] = 1))
# -0.6349
