import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addEvidence("A", 0)
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.4375  │ 0.5625  │
ie.chgEvidence("A", 1)
print(ie.posterior("C"))
#
#   C                │
# 0        │1        │
# ─────────│─────────│
#  0.6364  │ 0.3636  │
