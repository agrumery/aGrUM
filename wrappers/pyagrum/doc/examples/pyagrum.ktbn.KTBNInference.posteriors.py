import pyagrum as gum
import pyagrum.ktbn as ktbn

model = ktbn.KTBN.fromBN(gum.fastBN("X0->X1"))
model.cpt("X", 0).fillWith([0.5, 0.5])
model.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])  # P(X[t] | X[t-1])
ie = ktbn.KTBNInference(model)
ie.makeInference(3)
[[round(p, 4) for p in post.tolist()] for post in ie.posteriors("X")]
# [[0.5, 0.5], [0.55, 0.45], [0.585, 0.415]]
