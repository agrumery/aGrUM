import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.setEvidence({"C": 1})
ie.makeInference()
print(ie.posterior("A"))
#
#   A                │
# 0        │1        │
# ─────────│─────────│
#  0.6923  │ 0.3077  │
