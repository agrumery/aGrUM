import pyagrum as gum

mrf = gum.fastMRF("A--B;B--C")
mrf.factor({"A", "B"}).fillWith([4, 1, 1, 2])
mrf.factor({"B", "C"}).fillWith([1, 3, 2, 1])
ie = gum.ShaferShenoyMRFInference(mrf)
ie.addJointTarget({"A", "C"})
ie.jointTargets()
# [{0, 2}]
print(ie.jointPosterior({"A", "C"}).reorganize(["A", "C"]))  # fixed order of the variables
#
#       ║  A                │
# C     ║0        │1        │
# ──────║─────────│─────────│
# 0     ║ 0.2593  │ 0.2593  │
# 1     ║ 0.3333  │ 0.1481  │
