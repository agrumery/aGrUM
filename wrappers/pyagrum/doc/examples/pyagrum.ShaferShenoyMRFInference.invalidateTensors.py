import pyagrum as gum

mrf = gum.fastMRF("A--B")
mrf.factor({"A", "B"}).fillWith([1, 1, 1, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.5, 0.5]
mrf.factor({"A", "B"}).fillWith([3, 1, 3, 1])  # changed in place: the inference is not notified
ie.invalidateTensors()
ie.makeInference()
[round(p, 4) for p in ie.posterior("A").tolist()]
# [0.75, 0.25]
