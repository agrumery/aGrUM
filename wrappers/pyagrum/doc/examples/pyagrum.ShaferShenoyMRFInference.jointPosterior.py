import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addJointTarget({"A", "C"})
ie.setEvidence({"B": 1})
print(ie.jointPosterior({"A", "C"}).reorganize(["A", "C"]))  # fixed order of the variables
#
#       ║  A                │
# C     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.2500  │ 0.5000  │
# 1     ║ 0.0833  │ 0.1667  │
