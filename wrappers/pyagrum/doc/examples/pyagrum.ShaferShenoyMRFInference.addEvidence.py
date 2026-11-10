import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addEvidence("A", 1)  # hard evidence: A takes its value of index 1
ie.addEvidence("C", [0.2, 0.9])  # soft evidence: a likelihood on C
ie.nbrEvidence()
# 2
print(ie.posterior("B"))
#
#   B                │
# 0        │1        │
# ─────────│─────────│
#  0.4000  │ 0.6000  │
