import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
# hard evidence (index or label) and soft evidence (likelihood)
ie.setEvidence({"A": 1, "C": [0.2, 0.9]})
print(ie.posterior("B"))
#
#   B                │
# 0        │1        │
# ─────────│─────────│
#  0.4000  │ 0.6000  │
ie.setEvidence({"B": "0"})  # replaces all the previous evidence
sorted(ie.hardEvidenceNodes())
# [1]
